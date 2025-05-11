// FILE: main.c
// Program for lexical and syntax analysis using test files from tests directory

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/lexer.h"
#include "include/utils.h"
#include "include/parser.h"
#include "include/ad.h"

int main(int argc, char *argv[])
{
    char *inputFile = NULL;
    int useTableDisplay = 0;

    // Process command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-t") == 0)
        {
            useTableDisplay = 1;
        }
        else if (inputFile == NULL)
        {
            inputFile = argv[i];
        }
    }

    if (inputFile == NULL)
    {
        inputFile = "tests/testlex.c";
        printf("No input file specified. Using default: %s\n", inputFile);
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

    // Create global domain in the symbol table before parsing
    pushDomain();

    // Perform syntax analysis with domain analysis
    printf("\nStarting syntax analysis...\n");
    parse(tkList);
    printf("Syntax analysis completed successfully.\n");

    // Display the content of the global domain
    if (useTableDisplay)
    {
        printf("\nDisplaying domain analysis in table format:\n");
        showDomainTable(symTable, "global");
    }
    else
    {
        printf("\nDisplaying standard domain analysis:\n");
        showDomain(symTable, "global");
    }

    // Remove the global domain
    dropDomain();

    free(source);

    return 0;
}