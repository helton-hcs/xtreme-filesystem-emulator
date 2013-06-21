#define MAX_BUFFER_SIZE 1 << 7
#define FALSE 0
#define TRUE 1

struct fileNode {
	char *name;
	long size;
	//date, time
	struct directoryNode *directory;
};

struct directoryNode {
	char *name;
	struct directoryNode *subdirectories;
	struct fileNode *files;
}*root, *cwd;