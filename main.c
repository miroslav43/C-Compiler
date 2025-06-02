// FILE: main.c
// Program for lexical and syntax analysis using test files from tests directory

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/ad.h"
#include "include/at.h"
#include "include/utils.h"
#include "include/vm.h"
#include "include/gc.h"

int main(int argc, char *argv[])
{
    int showTable = 0; // Flag for table format display
    char *inputFile = NULL;

    // Process command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-t") == 0)
        {
            showTable = 1;
        }
        else
        {
            inputFile = argv[i];
        }
    }

    if (inputFile == NULL)
    {
        printf("Usage: %s [-t] <source_file>\n", argv[0]);
        printf("  -t   Show domain analysis in table format\n");
        exit(1);
    }

    // Load file content
    char *source = loadFile(inputFile);
    if (source == NULL)
    {
        printf("Cannot open file %s\n", inputFile);
        exit(1);
    }

    printf("Analyzing file: %s\n", inputFile);

    // Tokenize the source
    Token *tokens = tokenize(source);

    // Print tokens if not running the type analysis tests
    if (strstr(inputFile, "testat.c") == NULL)
    {
        showTokens(tokens);
    }

    // Initialize domain analysis
    pushDomain(); // Create global domain

    // Initialize VM for external functions - MOVED BEFORE PARSING
    vmInit();
    reset_results(); // Reset result collection

    // Register external functions used in test files
    Type intType = {TB_INT, NULL, -1, false};
    Type doubleType = {TB_DOUBLE, NULL, -1, false};
    Type charType = {TB_CHAR, NULL, -1, false};
    Type voidType = {TB_VOID, NULL, -1, false};
    Type stringType = {TB_CHAR, NULL, 0, false}; // char[] type for strings

    Symbol *putsSym = addExtFn("puts", (void (*)())puts1_wrapper, voidType);
    addFnParam(putsSym, "s", stringType);

    Symbol *putcSym = addExtFn("putc", (void (*)())putc1_wrapper, voidType);
    addFnParam(putcSym, "c", charType);

    // Run parser
    printf("\nStarting syntax analysis...\n");
    parse(tokens);
    printf("Syntax analysis completed successfully.\n");

    // Show domain analysis results BEFORE code execution
    if (strstr(inputFile, "testat.c") == NULL)
    {
        printf("\nDomain Analysis Results:\n");
        printf("========================\n");
        if (showTable)
        {
            showDomainTable(symTable, "global");
        }
        else
        {
            showDomain(symTable, "global");
        }
        printf("========================\n");
    }

    // Code generation and execution
    Symbol *symMain = findSymbolInDomain(symTable, "main");
    if (!symMain)
        err("missing main function");
    Instr *entryCode = NULL;
    addInstr(&entryCode, OP_CALL)->arg.instr = symMain->fn.instr;
    addInstr(&entryCode, OP_HALT);

    printf("\nStarting code execution...\n");
    printf("Program Output:\n");
    printf("===============\n");
    run(entryCode);
    printf("===============\n");
    printf("Code execution completed.\n");

    // Display collected results
    print_collected_results();

    // Cleanup
    dropDomain(); // Remove global domain
    free(source);

    printf("Compilation successful!\n");
    return 0;
}