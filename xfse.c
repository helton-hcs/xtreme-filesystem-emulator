#include "xfse.h"

struct directoryNode *newDirectoryNode(char *path) {
	return NULL;
}

void initRootNode() {
	rootNode = malloc(sizeof(struct directoryNode));
	rootNode->name = malloc(sizeof(char));
	strcpy(rootNode->name, "/");
	currentDirectory = rootNode;
}

void printPrompt() {
	printf("[xfse]%s>> ", currentDirectory->name);
}

char *getString(char *buffer, int length) {
	char *p;
	fgets(buffer, length, stdin);
	if ((p = strchr(buffer, '\n')) != NULL) {
		*p = '\0';
	}
	return buffer;
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
	       " # cd\t\t\t\tchange the working directory\n"
	       "   Usage: cd DIRECTORY\n"		
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
		   " # ls\t\t\t\tlist the directory files\n"
		   "   Usage: ls [OPTIONS]\n"
		   "     -a, --all\t\t\tall info detailed\n"
		   "---------------------------------------------------------------\n"
	       " # mkdir\t\t\tcreate a directory\n"
	       "   Usage: mkdir DIRECTORY\n"	       
		   "---------------------------------------------------------------\n"
	       " # mkf\t\t\t\tcreate a empty file\n"
	       "   Usage: mkf FILE [OPTIONS] \n"
	       "    -s, --size\t\t\tsize of file\n"		
		   "---------------------------------------------------------------\n"
	       " # mmap\t\t\t\tshow a memory map\n"
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
	       "   Usage: tree [OPTIONS] \n"
	       "     -d, -directory\t\tfirst directory of the tree\n"
		   "---------------------------------------------------------------\n"
	      );
}

void readCommand(char *command){
	if (feof(stdin)) {
		puts("exit");
		exit(EXIT_SUCCESS);
	}
	else {
		if (!strcmp(command, "exit") || !strcmp(command, "quit")) {
			exit(EXIT_SUCCESS);		
		}
		//puts(command);
		printHelp();
	}
}

void readInput() {
	char line[MAX_BUFFER_SIZE];
	do {
		printPrompt();
		readCommand(getString(line, MAX_BUFFER_SIZE));
	} while(TRUE);
}

void releaseResources() {
	free(rootNode);
}

void printWelcome() {
	printHeaderXFSE("Tip: Type \"help\" if you need any help");
}

int main(int argc, char **argv) {
	printWelcome();
	initRootNode();
	readInput();
	releaseResources();
	return EXIT_SUCCESS;
}