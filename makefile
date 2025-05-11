# Comprehensive Makefile for AtomC Compiler Project

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

# Directories
INCDIR = include
SRCDIR = src
TESTDIR = tests
BINDIR = bin
OBJDIR = $(BINDIR)/obj
TESTBINDIR = $(BINDIR)/tests

# Main executable name
TARGET = $(BINDIR)/atomc

# Source files (excluding main.c)
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Main file
MAIN_SRC = main.c
MAIN_OBJ = $(OBJDIR)/main.o

# Test files in tests directory
TEST_PARSER = $(TESTDIR)/testparser.c
TEST_LEXER = $(TESTDIR)/testlex.c
TEST_ERROR = $(TESTDIR)/error_test.c
TEST_DOMAIN = $(TESTDIR)/testad.c
TEST_DOMAIN_ERRORS = $(TESTDIR)/test_domain_errors.c

# Test executables
TEST_PARSER_BIN = $(TESTBINDIR)/test_parser
TEST_LEXER_BIN = $(TESTBINDIR)/test_lexer
TEST_ERROR_BIN = $(TESTBINDIR)/test_error
TEST_DOMAIN_BIN = $(TESTBINDIR)/test_domain

# Primary targets
.PHONY: all clean test test_lexer test_parser test_error test_domain test_domain_errors help

# Default target: build everything
all: directories $(TARGET) test_bins

# Create required directories
directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(TESTBINDIR)

# Compile the main executable
$(TARGET): $(OBJECTS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Create test binaries
test_bins: $(TEST_PARSER_BIN) $(TEST_LEXER_BIN) $(TEST_ERROR_BIN) $(TEST_DOMAIN_BIN)

# Compile the main.c file
$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Compile object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Special object files for test programs (to avoid name conflicts)
$(OBJDIR)/test_parser.o: $(TEST_PARSER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR)/test_lexer.o: $(TEST_LEXER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR)/test_error.o: $(TEST_ERROR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR)/test_domain.o: $(TEST_DOMAIN)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Build test executables
$(TEST_PARSER_BIN): $(MAIN_OBJ) $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS)

$(TEST_LEXER_BIN): $(MAIN_OBJ) $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS)

$(TEST_ERROR_BIN): $(MAIN_OBJ) $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS)

$(TEST_DOMAIN_BIN): $(MAIN_OBJ) $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS)

# Run test programs
test_parser: $(TEST_PARSER_BIN)
	@echo "=== Running parser test ==="
	$(TEST_PARSER_BIN) $(TEST_PARSER)

test_lexer: $(TEST_LEXER_BIN)
	@echo "=== Running lexer test ==="
	$(TEST_LEXER_BIN) $(TEST_LEXER)

test_error: $(TEST_ERROR_BIN)
	@echo "=== Running error test ==="
	$(TEST_ERROR_BIN) $(TEST_ERROR)

test_domain: $(TARGET)
	@echo "=== Running domain analysis test ==="
	$(TARGET) $(TEST_DOMAIN)

test_domain_errors: $(TARGET)
	@echo "=== Running domain analysis error test ==="
	$(TARGET) $(TEST_DOMAIN_ERRORS) || echo "Error detected as expected"

# Run all tests
test: test_lexer test_parser test_error test_domain
	@echo "=== All tests completed ==="

# Domain analysis tests
domain: test_domain test_domain_errors
	@echo "=== Domain analysis tests completed ==="

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BINDIR)
	@echo "Clean complete"

# Generate documentation
docs:
	@echo "Generating documentation..."
	@echo "# AtomC Compiler" > README.md
	@echo "" >> README.md
	@echo "## Description" >> README.md
	@echo "AtomC is a simple C-like language compiler that includes a lexical analyzer, parser, and domain analyzer." >> README.md
	@echo "" >> README.md
	@echo "## Building" >> README.md
	@echo "To build the compiler and test programs:" >> README.md
	@echo "\`\`\`bash" >> README.md
	@echo "make" >> README.md
	@echo "\`\`\`" >> README.md
	@echo "" >> README.md
	@echo "## Testing" >> README.md
	@echo "Various test targets are available:" >> README.md
	@echo "- \`make test_lexer\` - Run lexer tests" >> README.md
	@echo "- \`make test_parser\` - Run basic parser tests" >> README.md
	@echo "- \`make test_domain\` - Run domain analysis tests" >> README.md
	@echo "- \`make test_domain_errors\` - Run domain analysis error tests" >> README.md
	@echo "- \`make domain\` - Run all domain analysis tests" >> README.md
	@echo "- \`make test\` - Run all tests" >> README.md
	@echo "" >> README.md
	@echo "## Usage" >> README.md
	@echo "To run the compiler on a source file:" >> README.md
	@echo "\`\`\`bash" >> README.md
	@echo "./bin/atomc path/to/source/file.c" >> README.md
	@echo "\`\`\`" >> README.md
	@echo "Documentation generated."

# Show help message
help:
	@echo "AtomC Compiler - Makefile Help"
	@echo "============================="
	@echo ""
	@echo "Build targets:"
	@echo "  make                - Build everything (compiler and test programs)"
	@echo "  make clean          - Remove all build artifacts"
	@echo ""
	@echo "Test targets:"
	@echo "  make test_lexer     - Run lexer test (tests/testlex.c)"
	@echo "  make test_parser    - Run parser test (tests/testparser.c)"
	@echo "  make test_error     - Run error test (tests/error_test.c)"
	@echo "  make test_domain    - Run domain analysis test (tests/test_domain.c)"
	@echo "  make test_domain_errors - Run domain analysis error test (tests/test_domain_errors.c)"
	@echo "  make domain         - Run all domain analysis tests"
	@echo "  make test           - Run all tests"
	@echo ""
	@echo "Documentation:"
	@echo "  make docs           - Generate project documentation"
	@echo "  make help           - Display this help message"