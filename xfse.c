#include "xfse.h"

void emptySectors() {
	int i;
	for (i = 0; i < SECTOR_COUNT; i++) {
		sectorsMap[i] = EMPTY;
	}
}

void allocateSystemSectors() {
	int i;
	for (i = 0; i < SYSTEM_SECTORS_COUNT; i++) {
		sectorsMap[i] = FULL;
	}
}

void initDiskSectors() {
	emptySectors();
	allocateSystemSectors();
}

void printSectors() {
	int i;
	printf("\n# Sectors map:\n"
			"░ = Empty sector\n"
			"▓ = Full sector\n\n");
	for (i = 0; i < SECTOR_COUNT; i++) {
		printf("%s", sectorsMap[i] == FULL ? "▓" : "░");
	}
	printf("\n");
}

void printPrompt() {
	printf("[xfse]%s>> ", cwd->name);
}

void printHeaderXFSE(char *message) {
	printf("|========================================|\n"
		   "|   XFSE - eXtreme FileSystem Emulator   |\n"
		   "|========================================|\n"
		   " > %s\n\n", message);
}

void printHelp() {
	printHeaderXFSE("XFSE Help");
	printf(
		   "---------------------------------------------------------------\n"
//	       " # cd\t\t\t\tchange the working directory\n"
//	       "   Usage: cd DIRECTORY\n"
//		   "---------------------------------------------------------------\n"
	       " # clear\t\t\tclear screen\n"
	       "   Usage: clear\n"
		   "---------------------------------------------------------------\n"
	       " # exit\t\t\t\tterminate the XFSE program\n"	       	       
	       "   Usage: exit\n"
		   "---------------------------------------------------------------\n"
	       " # help\t\t\t\tthis help screen\n"
	       "   Usage: help\n"	 		   
		   "---------------------------------------------------------------\n"
//		   " # ls\t\t\t\tlist the directory files\n"
//		   "   Usage: ls [OPTIONS]\n"
//		   "     -a, --all\t\t\tall info detailed\n"
//		   "---------------------------------------------------------------\n"
	       " # mkdir\t\t\tcreate a directory\n"
	       "   Usage: mkdir DIRECTORY\n"	       
		   "---------------------------------------------------------------\n"
	       " # mkf\t\t\t\tcreate a empty file\n"
	       "   Usage: mkf FILE [OPTIONS] \n"
	       "    -s, --size\t\t\tsize of file\n"		
		   "---------------------------------------------------------------\n"
	       " # showmap\t\t\t\tshow a sectors' map used\n"
	       "   Usage: mmap\n"      	       
		   "---------------------------------------------------------------\n"
	       " # quit\t\t\t\tterminate the XFSE program\n"
	       "   Usage: quit\n"	       
		   "---------------------------------------------------------------\n"
	       " # rmdir\t\t\tremove a directory\n"
	       "   Usage: rmdir DIRECTORY\n"	       
		   "---------------------------------------------------------------\n"
	       " # rmf\t\t\t\tremove a file\n"
	       "   Usage: rmf FILE\n"	       
		   "---------------------------------------------------------------\n"
	       " # tree\t\t\t\tshow a directory tree\n"
//	       "   Usage: tree [OPTIONS] \n"
//	       "     -d, -directory\t\tfirst directory of the tree\n"
		   "---------------------------------------------------------------\n"
	      );
}

struct directoryNode *findSubDirectory(struct directoryNode *parentDirectory, char *name) {
	struct directoryNode *currentSubDirectory = parentDirectory->firstSubdirectory;
	while (currentSubDirectory) {
		if (!strcmp(currentSubDirectory->name, name)) {
			return currentSubDirectory;
		}
		currentSubDirectory = currentSubDirectory->nextDirectory;
	}
	return NULL;
}

struct directoryNode *findDirectoryByPath(char *path) {
	if (path) {
		struct directoryNode *currentNode = root;
		char *currentDir = copyString(path);
		currentDir = strtok(currentDir, "/");
		while (currentDir) {
			currentNode = findSubDirectory(currentNode, currentDir);
			if (!currentNode) {
				return NULL;
			}
			currentDir = strtok(NULL, "/");
		}
		return currentNode;
	}
	return NULL;
}

struct tm *getLocalTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return localtime(&tv.tv_sec);
}

char *getFormatedLocalTime(struct tm *ptm) {
	char timeString[40];
	strftime(timeString, sizeof(timeString), "%d/%m/%Y %H:%M:%S", ptm);
	return copyString(timeString);
}

struct directoryNode *newDirectoryNode() {
	struct directoryNode *dir = malloc(sizeof(struct directoryNode));
	dir->parentDirectory = dir->priorDirectory = dir->nextDirectory = dir->firstSubdirectory = dir->lastSubdirectory = NULL;
	dir->firstFile = dir->lastFile = NULL;
	dir->level = dir->size  = 0;
	dir->creationTime = getLocalTime();
	dir->modificationTime = getLocalTime();
	return dir;
}

struct directoryNode *createDirectory(struct directoryNode *parentDirectory, char *directoryName) {
	struct directoryNode *dir = newDirectoryNode();
	dir->name = copyString(directoryName);
	dir->parentDirectory = parentDirectory;
	if (parentDirectory) {
		dir->level = parentDirectory->level + 1;
		if (parentDirectory->lastSubdirectory) {
			dir->priorDirectory = parentDirectory->lastSubdirectory;
			parentDirectory->lastSubdirectory->nextDirectory = dir;
		}
		parentDirectory->lastSubdirectory = dir;
		if (!parentDirectory->firstSubdirectory) {
			parentDirectory->firstSubdirectory = dir;
		}
	}
	return dir;
}

void removeDirectory(struct directoryNode *directory) {
	if ((directory == directory->parentDirectory->firstSubdirectory) &&
  	    (directory == directory->parentDirectory->lastSubdirectory)) {
		directory->parentDirectory->firstSubdirectory = directory->parentDirectory->lastSubdirectory = NULL;
	}
	else if ((directory != directory->parentDirectory->firstSubdirectory) &&
			 (directory != directory->parentDirectory->lastSubdirectory)) {
		directory->priorDirectory->nextDirectory = directory->nextDirectory;
	}
	else if (directory == directory->parentDirectory->firstSubdirectory) {
		directory->parentDirectory->firstSubdirectory = directory->nextDirectory;
		directory->parentDirectory->firstSubdirectory->priorDirectory = NULL;
	}
	else if (directory == directory->parentDirectory->lastSubdirectory) {
		directory->parentDirectory->lastSubdirectory = directory->priorDirectory;
		directory->priorDirectory->nextDirectory = NULL;
	}
	free(directory);
}

void createRootDirectory() {
	root = createDirectory(NULL, "/");
	cwd = root;
}

char *extractSingleArgumentFrom(char *command) {
	char *arg = command;
	arg = strtok(arg,  " \t");
	arg = strtok(NULL, " \t");
	return arg;
}

char *getParentPath(char *path) {
	char *parentPath = copyString(path);
	int i;
	for (i = strlen(parentPath) - 1; i >= 0; i--) {
		if (parentPath[i] == '/') {
			parentPath[i] = 0;
			return parentPath;
		}
	}
	return NULL;
}

char *getDirectoryName(char *path) {
	char *parentPath = copyString(path);
	int i;
	for (i = strlen(parentPath) - 1; i >= 0; i--) {
		if (parentPath[i] == '/') {
			parentPath[i] = 0;
			parentPath = &parentPath[i+1];
			return parentPath;
		}
	}
	return NULL;
}

void mkdir(char *command) {
	char *path = extractSingleArgumentFrom(command);
	if (!findDirectoryByPath(path)) {
		char *parentPath = getParentPath(path);
		char *directoryName = getDirectoryName(path);
		struct directoryNode *parentDirectory = findDirectoryByPath(parentPath);
		if (!parentDirectory) {
			printf("Parent directory \"%s\" not found.\n", parentPath);
		}
		else {
			createDirectory(parentDirectory, directoryName);
		}
	}
	else {
		printf("The directory \"%s\" already exists.\n", path);
	}
}

int isDirectoryEmpty(struct directoryNode *directory) {
	return (!directory->firstSubdirectory && !directory->firstFile);
}

void rmdir(char *command) {
	char *path = extractSingleArgumentFrom(command);
	struct directoryNode *directory = findDirectoryByPath(path);
	if (directory) {
		if (directory == root) {
			printf("The root directory cannot be removed.\n");
		}
		else if (!isDirectoryEmpty(directory)) {
			printf("The directory \"%s\" is not empty. It cannot be removed.\n", path);
		}
		else {
			removeDirectory(directory);
		}
	}
	else {
		printf("The directory \"%s\" not exists.\n", path);
	}
}

void printDirectoriesRecursivelyFrom(struct directoryNode *currentDirectory) {
	int i = 0;
	if (currentDirectory) {
		printf("\n");
		for (i = 0; i < currentDirectory->level*2; i++) {
			putchar('-');
		}
		printf("%s", currentDirectory->name);
		struct directoryNode *currentSubDirectory = currentDirectory->firstSubdirectory;
		while (currentSubDirectory) {
			printDirectoriesRecursivelyFrom(currentSubDirectory);
			currentSubDirectory = currentSubDirectory->nextDirectory;
		}
	}
}

void printDirectoryTree() {
	printf("\n# Directory tree:");
	printDirectoriesRecursivelyFrom(root);
	printf("\n");
}

int isCommand(char *command, char *expectedCommand) {
	return !strncmp(command, expectedCommand, strlen(expectedCommand));
}

void readCommand(char *command){
	if (feof(stdin)) {
		puts("exit");
		exit(EXIT_SUCCESS);
	}
	else {
		if (isCommand(command, "exit") || isCommand(command, "quit")) {
			exit(EXIT_SUCCESS);		
		}
		else if (isCommand(command, "mkdir")) {
			mkdir(command);
		}
		else if (isCommand(command, "rmdir")) {
			rmdir(command);
		}
		else if (isCommand(command, "help")) {
			printHelp();
		}
		else if (isCommand(command, "tree")) {
			printDirectoryTree();
		}
		else if (isCommand(command, "showmap")) {
			printSectors();
		}
		else if (isCommand(command, "clear")) {
			system("clear");
		}
		else {
			printf("Unrecognized command. Type \"help\" if you don't know the emulator commands.\n");
		}
	}
}

void readInput() {
	char line[MAX_BUFFER_SIZE];
	do {
		printPrompt();
		readCommand(getString(line, MAX_BUFFER_SIZE));
	} while(TRUE);
}

void printWelcome() {
	printHeaderXFSE("Tip: Type \"help\" if you need any help");
}

int main(int argc, char **argv) {
	printWelcome();
	createRootDirectory();
	initDiskSectors();
	readInput();
	return EXIT_SUCCESS;
}
