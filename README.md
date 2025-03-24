# Lexical Analyzer

## Description
This is a lexical analyzer for a simple programming language. It tokenizes source code and identifies language constructs.

## Compilation
To compile the project, run:
```
make
```

## Usage
To analyze a source file:
```
./lexer path/to/source/file.c
```

## Makefile Targets
- `make` - Build the lexer executable
- `make test` - Run the lexer with the standard test file
- `make error` - Run the lexer with the error test file
- `make run file=<filepath>` - Run with a specific input file
- `make clean` - Remove compiled files
- `make docs` - Generate this README file

## Supported Tokens
- Keywords: if, else, while, return, struct, void, int, double, char
- Operators: +, -, *, /, =, ==, !=, <, >, <=, >=, &&, ||, !
- Delimiters: (, ), [, ], {, }, ;, ,
- Literals: integers, floating-point numbers, characters, strings
- Identifiers: names for variables, functions, etc.

