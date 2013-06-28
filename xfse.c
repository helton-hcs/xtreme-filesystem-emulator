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

void showmap() {
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

void help() {
	printHeaderXFSE("XFSE Help");
	printf(
		   "---------------------------------------------------------------\n"
	       " # clear\t\t\tclear screen\n"
	       "   Usage: clear\n"
		   "---------------------------------------------------------------\n"
	       " # exit\t\t\t\tterminate the XFSE program\n"	       	       
	       "   Usage: exit\n"
		   "---------------------------------------------------------------\n"
	       " # help\t\t\t\tthis help screen\n"
	       "   Usage: help\n"	 		   
		   "---------------------------------------------------------------\n"
		   " # ls\t\t\t\tlist the directory "
		   "   Usage: ls \n"
		   "---------------------------------------------------------------\n"
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
	       "   Usage: tree\n"
		   "---------------------------------------------------------------\n"
	      );
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

char *getLastNameFromPath(char *path) {
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

struct fileNode *findFileByPath(char *path) {
	if (path) {
		char *fileName = getLastNameFromPath(path);
		char *parentPath = getParentPath(path);
		struct directoryNode *parentDirectory = findDirectoryByPath(parentPath);
		struct fileNode *file = parentDirectory->firstFile;
		while(file) {
			if (!strcmp(file->name, fileName)) {
				return file;
			}
			file = file->nextFile;
		}
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
	dir->level = dir->size = dir->fileCount = 0;
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

struct fileNode *newFileNode() {
	struct fileNode *file = malloc(sizeof(struct fileNode));
	file->size = 0;
	file->creationTime = getLocalTime();
	file->modificationTime = getLocalTime();
	return file;
}

struct fileNode *createFile(struct directoryNode *directory, char *fileName, int fileSize) {
	struct fileNode *file = newFileNode();
	file->size = fileSize;
	file->name = copyString(fileName);
	if (directory->lastFile) {
		file->priorFile = directory->lastFile;
		directory->lastFile->nextFile = file;
	}
	directory->lastFile = file;
	if (!directory->firstFile) {
		directory->firstFile = file;
	}
	directory->size += fileSize;
	directory->fileCount++;
	directory->modificationTime = getLocalTime();
	//allocSpaceToFile(file);
	return file;
}

void removeFile(struct fileNode *file) {
	if ((file == file->directory->firstFile) &&
  	    (file == file->directory->lastFile)) {
		file->directory->firstFile = file->directory->lastFile = NULL;
	}
	else if ((file != file->directory->firstFile) &&
			 (file != file->directory->lastFile)) {
		file->priorFile->nextFile = file->nextFile;
	}
	else if (file == file->directory->firstFile) {
		file->directory->firstFile = file->nextFile;
		file->directory->firstFile->priorFile = NULL;
	}
	else if (file == file->directory->lastFile) {
		file->directory->lastFile = file->priorFile;
		file->priorFile->nextFile = NULL;
	}
	free(file);
}

void printDirectory(struct directoryNode *dir) {
	struct directoryNode *currentDir = dir->firstSubdirectory;
	struct fileNode *currentFile = dir->firstFile;
	int dirCount, fileCountDir, fileCountSubDir;
	dirCount = fileCountDir = fileCountSubDir = 0;
	while (currentDir) {
		dirCount++;
		fileCountSubDir += currentDir->fileCount;
		printf("%6d\t%s\t%s\t"KBLUE"%s"KNORMAL"\n", currentDir->size, getFormatedLocalTime(currentDir->creationTime),
				getFormatedLocalTime(currentDir->modificationTime), currentDir->name);
		currentDir = currentDir->nextDirectory;

	}
	while (currentFile) {
		fileCountDir++;
		printf("%6d\t%s\t%s\t%s\n", currentFile->size, getFormatedLocalTime(currentFile->creationTime),
				getFormatedLocalTime(currentFile->modificationTime), currentFile->name);
		currentFile = currentFile->nextFile;
	}
	printf("\n%d subdirectorie(s), %d file(s) [%d in the directory, %d in the subdirectories]"
		   "\n%d bytes used\n", dirCount, fileCountDir+fileCountSubDir, fileCountDir, fileCountSubDir, dir->size);
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

int isDirectoryEmpty(struct directoryNode *directory) {
	return (!directory->firstSubdirectory && !directory->firstFile);
}

void mkf(char *command) {
	char *path = extractSingleArgumentFrom(command);
	char *fileName = getLastNameFromPath(path);
	char *parentPath = getParentPath(path);
	struct directoryNode *parentDirectory = findDirectoryByPath(parentPath);
	if (!parentDirectory) {
		printf("Parent directory \"%s\" not found.\n", parentPath);
	}
	else {
		if (fileName) {
			int fileSize;
			printf("=> Type the file size: ");
			scanf("%d", &fileSize);
			setbuf(stdin, NULL);
			//check available space
			createFile(parentDirectory, fileName, fileSize);
		}
		else {
			printf("File name not found.\n");
		}
	}
}

void rmf(char *command) {
	char *path = extractSingleArgumentFrom(command);
	struct fileNode *file = findFileByPath(path);
	if (file) {
		removeFile(file);
	}
	else {
		printf("The file \"%s\" was not found.\n", path);
	}
}

void mkdir(char *command) {
	char *path = extractSingleArgumentFrom(command);
	if (!findDirectoryByPath(path)) {
		char *parentPath = getParentPath(path);
		char *directoryName = getLastNameFromPath(path);
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

void ls(char *command) {
	char *path = extractSingleArgumentFrom(command);
	struct directoryNode *directory = findDirectoryByPath(path);
	if (directory) {
		printDirectory(directory);
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

void tree() {
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
			help();
		}
		else if (isCommand(command, "tree")) {
			tree();
		}
		else if (isCommand(command, "showmap")) {
			showmap();
		}
		else if (isCommand(command, "clear")) {
			system("clear");
		}
		else if (isCommand(command, "ls")) {
			ls(command);
		}
		else if (isCommand(command, "mkf")) {
			mkf(command);
		}
		else if (isCommand(command, "rmf")) {
			rmf(command);
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
