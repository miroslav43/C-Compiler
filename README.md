# AtomC Compiler

A C-like language compiler that includes a lexical analyzer, parser, and domain analyzer. The compiler performs syntax analysis and domain analysis to ensure code correctness and type safety.

## Features

- Lexical Analysis: Tokenizes source code into meaningful units
- Syntax Analysis: Parses tokens into an abstract syntax tree
- Domain Analysis: Performs semantic checks including:
  - Unique struct names
  - Unique variable names within scopes
  - Array dimension validation
  - Type checking
  - Symbol table management
  - Pointer type support

## Project Structure

```
.
├── bin/                    # Compiled binaries
│   ├── obj/               # Object files
│   └── tests/             # Test executables
├── include/               # Header files
├── src/                   # Source files
├── tests/                 # Test files
│   ├── testparser.c      # Parser tests
│   ├── testlex.c         # Lexer tests
│   ├── error_test.c      # Error handling tests
│   ├── test_domain.c     # Domain analysis tests
│   ├── test_domain_errors.c # Domain analysis error tests
│   ├── test_domain_table.c # Domain analysis table display test
│   └── test_domain_table_main.c # Main program for table display
└── Makefile              # Build configuration
```

## Building

To build the compiler and test programs:

```bash
make
```

This will create the following:
- Main compiler executable in `bin/atomc`
- Test executables in `bin/tests/`

## Testing

The project includes several test targets:

- `make test_lexer` - Run lexer tests
- `make test_parser` - Run basic parser tests
- `make test_error` - Run error handling tests
- `make test_domain` - Run domain analysis tests
- `make test_domain_errors` - Run domain analysis error tests
- `make test_domain_table` - Run domain analysis with table format display
- `make domain` - Run all domain analysis tests
- `make test` - Run all tests

## Usage

To compile an AtomC source file:

```bash
./bin/atomc path/to/source/file.c
```

## Domain Analysis Features

The compiler performs the following domain analysis checks:

1. Struct Definitions:
   - Ensures unique struct names
   - Validates struct member declarations
   - Checks struct member access

2. Variable Declarations:
   - Enforces unique variable names within scopes
   - Validates array dimensions
   - Checks type compatibility
   - Supports pointer types (e.g., int*, struct Point*)

3. Function Definitions:
   - Validates function parameters
   - Checks return types
   - Ensures proper scoping of local variables

4. Type System:
   - Supports basic types (int, double, char, void)
   - Handles struct types
   - Manages array types with fixed dimensions
   - Supports pointer types

## Domain Analysis Display

The domain analysis results can be displayed in two formats:

### Standard Display

The standard display shows the domain analysis as a hierarchical tree structure:

```
// domain: Global
int globalVar;	// size=4, mem=0x7feb35c057b0
struct Point{
	int x;	// size=4, idx=0
	int y;	// size=4, idx=4
	};	// size=8
```

### Table Display

The table display provides a more structured view of domain analysis results:

```
╔══════════════════════════════════════════════════════════════════╗
║                    DOMAIN ANALYSIS: Global                       ║
╠══════════════════╦══════════════╦═══════════╦═══════════╦═════════╣
║ Name             ║ Kind         ║ Type      ║ Size      ║ Scope   ║
╠══════════════════╬══════════════╬═══════════╬═══════════╬═════════╣
║ Point            ║ struct       ║ struct    ║ 8 bytes   ║ global  ║
╠══════════════════╬══════════════╬═══════════╬═══════════╬═════════╣
║   Members:                                                       ║
║   x              ║ member       ║ int       ║ 4         ║         ║
║   y              ║ member       ║ int       ║ 4         ║         ║
╚══════════════════╩══════════════╩═══════════╩═══════════╩═════════╝
```

You can enable table display by using the `test_domain_table` target.

## Recent Improvements

- Added table-formatted domain analysis display
- Added support for pointer types (int*, struct Point*)
- Enhanced the lexer to handle the address-of operator (&)

## Development

### Prerequisites

- GCC compiler
- Make build system

### Building from Source

1. Clone the repository
2. Run `make` to build the project
3. Run `make test` to verify the build

### Clean Build

To remove all build artifacts:

```bash
make clean
```

## License

This project is part of a compiler design course and is intended for educational purposes.

## Contributing

This is an educational project. Feel free to use it as a reference for learning about compiler design and implementation.

