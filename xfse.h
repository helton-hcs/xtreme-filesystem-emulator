#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1 << 7
#define TRUE 1

struct directoryNode {
	char *name;
	struct directoryNode *next;
}*rootNode, *currentDirectory;

