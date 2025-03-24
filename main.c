// FILE: main.c
// Program for lexical analysis using test files from tests directory

#include <stdio.h>
#include <stdlib.h>
#include "include/lexer.h"
#include "include/utils.h"

int main(int argc, char *argv[])
{
    char *inputFile;

    if (argc < 2)
    {
        inputFile = "tests/testlex.c";
        printf("No input file specified. Using default: %s\n", inputFile);
    }
    else
    {
        inputFile = argv[1];
    }

    char *source = loadFile(inputFile);
    if (source == NULL)
    {
        fprintf(stderr, "Could not load file: %s\n", inputFile);
        return 1;
    }

    printf("Analyzing file: %s\n", inputFile);

    tokens = NULL;
    lastTk = NULL;
    line = 1;

    Token *tkList = tokenize(source);
    showTokens(tkList);

    free(source);


    return 0;
}