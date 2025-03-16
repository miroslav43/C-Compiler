// FILE: testlex.c
// Program de testare a analizorului lexical, v1.1

#include <stdio.h>
#include <stdlib.h>
#include "../include/lexer.h"
#include "../include/utils.h"

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
		return 1;
	}

	char *source = loadFile(argv[1]);
	if (source == NULL)
	{
		fprintf(stderr, "Could not load file: %s\n", argv[1]);
		return 1;
	}

	tokens = NULL;
	lastTk = NULL;
	line = 1;

	Token *tkList = tokenize(source);
	showTokens(tkList);

	free(source);

	return 0;
}