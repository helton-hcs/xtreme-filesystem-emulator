#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "utils.h"
#include "nodes.h"

void printPrompt();
void printHeaderXFSE(char *);
void printHelp();
void readCommand(char *);
void readInput();
void releaseResources();
void printWelcome();

void initRootNode();