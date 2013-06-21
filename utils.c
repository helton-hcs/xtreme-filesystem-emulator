#include "utils.h"

char *copyString(char *source) {
	char *dest = NULL;
	if (source) {
		dest = malloc(strlen(source));
		strcpy(dest, source);
	}
	return dest;
}

char *getString(char *buffer, int length) {
	char *p;
	fgets(buffer, length, stdin);
	if ((p = strchr(buffer, '\n')) != NULL) {
		*p = '\0';
	}
	return buffer;
}