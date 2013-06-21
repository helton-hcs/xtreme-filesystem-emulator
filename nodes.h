#include "types.h"

//TODO -uHelton -mCreate this functions
struct directoryNode *createDirectory(char *path, struct directoryNode *parentNode);
struct directoryNode *removeDirectory(char *path, struct directoryNode *parentNode/*, int recursiveRemove*/);
struct directoryNode *getDirectoryByPath(char *path);
long getDirectorySize(struct directoryNode *directory);

struct fileNode *createFile(char *path, long fileSize);
struct fileNode *removeFile(char *path);
struct fileNode *getFileByPath(char *path);