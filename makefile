# Comprehensive Makefile for AtomC Compiler Project

# Compiler and compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I $(INCDIR)
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

# Test executables
TEST_PARSER_BIN = $(TESTBINDIR)/test_parser
TEST_LEXER_BIN = $(TESTBINDIR)/test_lexer
TEST_ERROR_BIN = $(TESTBINDIR)/test_error
TEST_DOMAIN_BIN = $(TESTBINDIR)/test_domain

# Primary targets
.PHONY: all clean test test_lexer test_parser test_error test_domain domain help test_analiza test_clean docs test_analiza2 test_analiza_clean test-vm vm-activity6 vm-double-test test_gen

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
	$(CC) $(CFLAGS) -c $< -o $@

# Compile object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Special object files for test programs (to avoid name conflicts)
$(OBJDIR)/test_parser.o: $(TEST_PARSER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/test_lexer.o: $(TEST_LEXER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/test_error.o: $(TEST_ERROR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/test_domain.o: $(TEST_DOMAIN)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test executables
$(TEST_PARSER_BIN): $(MAIN_OBJ) $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS)

$(TEST_LEXER_BIN): $(MAIN_OBJ) $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS)

$(TEST_ERROR_BIN): $(MAIN_OBJ) $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS)

$(TEST_DOMAIN_BIN): $(MAIN_OBJ) $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) $(OBJECTS) $(LDFLAGS)

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

test_domain_standard: $(TARGET)
	@echo "=== Running domain analysis with standard output ==="
	$(TARGET) $(TEST_DOMAIN)

test_domain_table: $(TARGET)
	@echo "=== Running domain analysis with table output ==="
	$(TARGET) $(TEST_DOMAIN) -t

# Type analysis tests
test_analiza: $(TARGET)
	@echo "Running type analysis tests..."
	./$(TARGET) tests/testat.c || true
	@echo "Analysis test done."

test_analiza2: $(TARGET)
	@echo "Running type analysis tests on testat2.c..."
	./$(TARGET) tests/testat2.c || true
	@echo "Analysis test 2 done."

test_analiza_clean: $(TARGET)
	@echo "Running type analysis tests on clean test file..."
	./$(TARGET) tests/testat_clean.c || true
	@echo "Clean analysis test done."

# Code generation tests
test_gen: $(TARGET)
	@echo "=== Running code generation tests ==="
	@echo "Testing simple variable assignment and output..."
	./$(TARGET) tests/simple_test.c || true
	@echo ""
	@echo "Testing complete factorial program with recursion and loops..."
	./$(TARGET) tests/testgc.c || true
	@echo "=== Code generation tests completed ==="

test_clean:
	@echo "Running type analysis with a clean test file..."
	sed -i.bak '25,35d' tests/testat.c
	./$(TARGET) tests/testat.c
	mv tests/testat.c.bak tests/testat.c

# Run all tests
test: test_lexer test_parser test_error test_domain
	@echo "=== All tests completed ==="

# Domain analysis tests
domain: test_domain_standard test_domain_table
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
	@echo "AtomC is a simple C-like language compiler that includes a lexical analyzer, parser, domain analyzer, and type analyzer." >> README.md
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
	@echo "- \`make test_domain_standard\` - Run domain analysis with standard output" >> README.md
	@echo "- \`make test_domain_table\` - Run domain analysis with table display" >> README.md
	@echo "- \`make test_analiza\` - Run type analysis tests with intentional errors" >> README.md
	@echo "- \`make test_analiza2\` - Run type analysis tests on testat2.c" >> README.md
	@echo "- \`make test_analiza_clean\` - Run type analysis tests on clean test file" >> README.md
	@echo "- \`make test_gen\` - Run code generation tests with VM execution" >> README.md
	@echo "- \`make test_clean\`
	@echo "- \`make vm-activity6\` - Run Activity 6 VM tests" >> README.md
	@echo "- \`make vm-double-test\` - Run only the double function test" >> README.md

# VM test target
bin/tests/test_vm: bin/obj/vm.o bin/obj/ad.o bin/obj/utils.o tests/test_vm.c
	$(CC) -Wall -Wextra -g -I include -o bin/tests/test_vm bin/obj/vm.o bin/obj/ad.o bin/obj/utils.o tests/test_vm.c

test-vm: bin/tests/test_vm
	./bin/tests/test_vm

# Activity 6 specific target - runs only the VM with the double function test
vm-activity6: bin/tests/test_vm
	@echo "=== Running Activity 6 VM tests ==="
	@echo "Testing virtual machine implementation with double operations"
	@echo "Running f(2.0) with double operations..."
	./bin/tests/test_vm
	@echo "=== Activity 6 VM tests completed ==="

# Create a specific test for the double function test
vm-double-test: bin/obj/vm.o bin/obj/ad.o bin/obj/utils.o
	@echo "=== Running only the double function test ==="
	@echo "Testing void f(double n) implementation..."
	@echo "Program: f(2.0);"
	@echo "void f(double n){"
	@echo "    double i=0.0;"
	@echo "    while(i<n){"
	@echo "        put_d(i);"
	@echo "        i=i+0.5;"
	@echo "    }"
	@echo "}"
	@echo ""
	@echo '#include <stdio.h>' > $(TESTDIR)/double_test.c
	@echo '#include "../include/vm.h"' >> $(TESTDIR)/double_test.c
	@echo '#include "../include/ad.h"' >> $(TESTDIR)/double_test.c
	@echo '' >> $(TESTDIR)/double_test.c
	@echo 'int main()' >> $(TESTDIR)/double_test.c
	@echo '{' >> $(TESTDIR)/double_test.c
	@echo '    printf("\\n\\nRunning double test program (genTestDoubleProgram):\\n");' >> $(TESTDIR)/double_test.c
	@echo '    pushDomain();' >> $(TESTDIR)/double_test.c
	@echo '    vmInit();' >> $(TESTDIR)/double_test.c
	@echo '    Instr *testCodeD = genTestDoubleProgram();' >> $(TESTDIR)/double_test.c
	@echo '    run(testCodeD);' >> $(TESTDIR)/double_test.c
	@echo '    dropDomain();' >> $(TESTDIR)/double_test.c
	@echo '    return 0;' >> $(TESTDIR)/double_test.c
	@echo '}' >> $(TESTDIR)/double_test.c
	@mkdir -p $(TESTBINDIR)
	$(CC) -Wall -Wextra -g -I include -o bin/tests/double_test bin/obj/vm.o bin/obj/ad.o bin/obj/utils.o $(TESTDIR)/double_test.c
	./bin/tests/double_test
	@echo "=== Double function test completed ==="