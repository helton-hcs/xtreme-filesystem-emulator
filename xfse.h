#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "types.h"
#include "utils.h"

void printPrompt();
void printHeaderXFSE(char *);
void printHelp();
void readCommand(char *);
void readInput();
void releaseResources();
void printWelcome();

void initRootNode();

int sectorsMap[SECTOR_COUNT];
