# Makefile for lexical analyzer project

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
INCDIR = include
SRCDIR = src
TESTDIR = tests
BINDIR = bin

# Source files and object files
SRC_FILES = $(SRCDIR)/lexer.c $(SRCDIR)/utils.c main.c
OBJ_FILES = $(patsubst %.c,$(BINDIR)/%.o,$(notdir $(SRC_FILES)))

# Executable name
TARGET = lexer

# Default target
all: $(TARGET)

# Make sure bin directory exists
$(BINDIR):
	mkdir -p $(BINDIR)

# Link object files to create executable
$(TARGET): $(BINDIR) $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $(OBJ_FILES)

# Compile source files into object files
$(BINDIR)/%.o: $(SRCDIR)/%.c | $(BINDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Handle main.c compilation
$(BINDIR)/main.o: main.c | $(BINDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Run the lexer with the default test file
test: $(TARGET)
	./$(TARGET) $(TESTDIR)/testlex.c

# Run the lexer with the error test file
error: $(TARGET)
	./$(TARGET) $(TESTDIR)/error_test.c

# Run with custom input file
run: $(TARGET)
	@echo "Running lexer with specified input file"
	./$(TARGET) $(file)

# Clean up compiled files
clean:
	rm -rf $(BINDIR) $(TARGET)

# Generate a README file with basic documentation
docs:
	@echo "# Lexical Analyzer" > README.md
	@echo "" >> README.md
	@echo "## Description" >> README.md
	@echo "This is a lexical analyzer for a simple programming language. It tokenizes source code and identifies language constructs." >> README.md
	@echo "" >> README.md
	@echo "## Compilation" >> README.md
	@echo "To compile the project, run:" >> README.md
	@echo "\`\`\`" >> README.md
	@echo "make" >> README.md
	@echo "\`\`\`" >> README.md
	@echo "" >> README.md
	@echo "## Usage" >> README.md
	@echo "To analyze a source file:" >> README.md
	@echo "\`\`\`" >> README.md
	@echo "./lexer path/to/source/file.c" >> README.md
	@echo "\`\`\`" >> README.md
	@echo "" >> README.md
	@echo "## Makefile Targets" >> README.md
	@echo "- \`make\` - Build the lexer executable" >> README.md
	@echo "- \`make test\` - Run the lexer with the standard test file" >> README.md
	@echo "- \`make error\` - Run the lexer with the error test file" >> README.md
	@echo "- \`make run file=<filepath>\` - Run with a specific input file" >> README.md
	@echo "- \`make clean\` - Remove compiled files" >> README.md
	@echo "- \`make docs\` - Generate this README file" >> README.md
	@echo "" >> README.md
	@echo "## Supported Tokens" >> README.md
	@echo "- Keywords: if, else, while, return, struct, void, int, double, char" >> README.md
	@echo "- Operators: +, -, *, /, =, ==, !=, <, >, <=, >=, &&, ||, !" >> README.md
	@echo "- Delimiters: (, ), [, ], {, }, ;, ," >> README.md
	@echo "- Literals: integers, floating-point numbers, characters, strings" >> README.md
	@echo "- Identifiers: names for variables, functions, etc." >> README.md
	@echo "" >> README.md
	@echo "README.md generated successfully."

# Display help information
help:
	@echo "Lexical Analyzer - Makefile Help"
	@echo "---------------------------------"
	@echo "Available targets:"
	@echo "  make         - Build the lexer executable"
	@echo "  make test    - Run the lexer with the standard test file"
	@echo "  make error   - Run the lexer with the error test file"
	@echo "  make run file=<filepath> - Run with a specific input file"
	@echo "  make clean   - Remove compiled files"
	@echo "  make docs    - Generate README.md documentation"
	@echo "  make help    - Display this help message"

.PHONY: all test error clean help run docs