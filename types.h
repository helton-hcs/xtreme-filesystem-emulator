#include <sys/time.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1 << 7
#define FALSE 0
#define TRUE 1
#define SYSTEM_SECTORS_COUNT 10
#define SECTOR_COUNT 256
#define SECTOR_SIZE 512
#define EMPTY 0
#define FULL 1

struct fileNode {
	char *name;
	long size;
	struct tm *creationTime, *modificationTime;
	struct directoryNode *directory;
	struct fileNode *priorFile, *nextFile;
};

struct directoryNode {
	char *name;
	struct directoryNode *parentDirectory, *priorDirectory, *nextDirectory, *firstSubdirectory, *lastSubdirectory;
	struct fileNode *firstFile, *lastFile;
	int level, size;
	struct tm *creationTime, *modificationTime;
}*root, *cwd;
