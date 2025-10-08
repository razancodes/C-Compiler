# C-Compiler

This is a C compiler project developed to deepen understanding of compiler design and low-level programming. The compiler supports basic arithmetic operations and demonstrates the core phases of compilation from lexical analysis to code generation.

## Overview

The project implements key compiler components:
- Lexer: Tokenizes input C source code
- Parser: Builds Abstract Syntax Trees (AST) from tokens
- AST: Data structures representing program structure
- Code Generator: Converts AST to machine-level instructions

Designed for educational purposes, the modular source files enable study and extension of each compiler phase.

## Repository Structure

- `lexer.c` / `lexer.h`: Lexer implementation for tokenizing input
- `parser.c` / `parser.h`: Parser building the AST
- `ast.c` / `ast.h`: AST node definitions and utilities
- `codegen.c` / `codegen.h`: Code generator producing executable code
- `main.c`: Entry point controlling compilation process
- `test.c`: Sample file to test the compiler

## Features

- Supports basic integer arithmetic operations: `+`, `-`, `*`, `/`
- Tokenizes simple C syntax
- Builds and traverses AST
- Modular design for compiler components

## Getting Started

### Prerequisites

Use MSYS2 MinGW64 Compiler 

### Build
- Clone the repo: 
`git clone https://github.com/razancodes/C-Compiler.git`
`cd C-Compiler`
- Configure test.c for the expression you want: 
- Compile our compiler's C source files
`gcc -o razancompiler lexer.c parser.c ast.c codegen.c main.c`
`./razancompiler test.c`
- Assemble the generated output.s into an object file
`as -o output.o output.s`
- Link the object file into an executable
`gcc -o rznprogram output.o`

### Usage
- Run the compiled program
Execute Finally as: 
`./rznprogram`
- Use `echo $?` To get the return value of the program.

## Example Usage

The compiler currently handles simple arithmetic in C like:
`test.c:
int main() {return 2+3;}`
You can compile similar code with this compiler while studying how basic expressions are processed.

## Learning Goals

This project provides practical insights into:

- Compiler internals such as tokenization, parsing, and code generation.
- Abstract Syntax Tree (AST) design and traversal.
- Building a minimal but functional compilation pipeline.
- Low-level programming concepts underlying high-level languages.

## Limitations

- Only basic arithmetic operations are supported.
- No advanced C features like pointers, control flow, or functions.
- Minimal error handling and diagnostic messages.
- Code optimizations are not implemented.

## Future Work

Potential extensions to improve this compiler:

- Support for control flow constructs (`if`, `while`, `for`).
- Function definitions and calls support.
- Enhanced type checking and semantic analysis.
- More extensive error reporting.
- Target assembly or machine code generation improvements.

## Contributing

Contributions to enhance the compiler or extend supported features are welcome:))
Feel free to open issues or submit pull requests if something doesnt work...

## License

Open source for educational and experimental use.

## Author
**razancodes**

