#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "ast.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }

    // Read source code from file
    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not open source file '%s'\n", argv[1]);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* source_code = (char*)malloc(file_size + 1);
    if (!source_code) {
        fprintf(stderr, "Memory allocation failed for source code.\n");
        fclose(fp);
        return 1;
    }
    fread(source_code, 1, file_size, fp);
    source_code[file_size] = '\0';
    fclose(fp);

    printf("--- Source Code ---\n%s\n", source_code);

    // Phase 1: Lexical Analysis
    lexer_init(source_code);
    printf("--- Lexing Initialized ---\n");

    // Phase 2 & 3: Syntax Analysis and AST Construction
    // The parser will call getNextToken internally.
    // Advance once to get the first token for the parser.
    advance(); 
    ASTNode* program_ast = parse_program();
    printf("--- Parsing and AST Construction Complete ---\n");
    printf("--- Generated AST ---\n");
    ast_print(program_ast, 0); // Print AST for verification

    // Phase 4: Code Generation
    printf("--- Generating Assembly Code ---\n");
    
    // Redirect stdout to a file for assembly output
    FILE* assembly_fp = fopen("output.s", "w");
    if (!assembly_fp) {
        fprintf(stderr, "Error: Could not open output assembly file.\n");
        ast_free(program_ast);
        free(source_code);
        return 1;
    }
    
        // Temporarily redirect stdout to file
        // Save the current stdout file descriptor
    int original_stdout_fd = dup(fileno(stdout));
    if (original_stdout_fd == -1) {
        fprintf(stderr, "Error: Could not duplicate stdout file descriptor.\n");
        ast_free(program_ast);
        free(source_code);
        fclose(assembly_fp);
        return 1;
    }

    // Redirect stdout to the file
    if (dup2(fileno(assembly_fp), fileno(stdout)) == -1) {
        fprintf(stderr, "Error: Could not redirect stdout.\n");
        ast_free(program_ast);
        free(source_code);
        fclose(assembly_fp);
        close(original_stdout_fd);
        return 1;
    }

    generate_code(program_ast);

    // Flush and restore stdout
    fflush(stdout);
    dup2(original_stdout_fd, fileno(stdout));
    close(original_stdout_fd);
    fclose(assembly_fp);

    printf("--- Assembly Code Generated to output.s ---\n");

    // Clean up AST and source code memory
    ast_free(program_ast);
    free(source_code);

    printf("Compilation successful!\n");
    return 0;
}