// codegen.c (simplified snippet)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "token.h" // For TokenType

// Helper function to emit assembly (e.g., print to stdout or file)
#define emitf printf

// Current stack offset for local variables (simple approach)
// Note: For a real compiler, this would be managed per function via a symbol table.
static int current_stack_offset = 0; // Tracks stack usage for push/pop for expressions

// Function to generate code for expressions
void generate_expression_code(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_NUMBER:
            emitf("  mov rax, %d\n", node->data.number.value);
            break;
        case AST_IDENTIFIER:
            // For this simple compiler, identifiers in expressions are not directly supported
            // as variables, only as part of function calls.
            // A symbol table would be needed to lookup variable offsets/registers.
            fprintf(stderr, "Code Generation Error: Identifiers in expressions (as variables) not yet supported.\n");
            exit(1);
            break;
        case AST_BINARY_OP:
            generate_expression_code(node->data.binary_op.left);
            emitf("  push rax\n"); // Save left operand on stack
            current_stack_offset += 8; // Adjust simulated stack offset
            generate_expression_code(node->data.binary_op.right);
            emitf("  pop rbx\n"); // Load left operand into rbx
            current_stack_offset -= 8; // Adjust simulated stack offset

            switch (node->data.binary_op.op) {
                case TOKEN_PLUS:
                    emitf("  add rax, rbx\n");
                    break;
                case TOKEN_MINUS:
                    emitf("  sub rbx, rax\n"); // rbx - rax
                    emitf("  mov rax, rbx\n"); // Result into rax
                    break;
                case TOKEN_MULTIPLY:
                    emitf("  imul rax, rbx\n"); // rax = rax * rbx
                    break;
                case TOKEN_DIVIDE:
                    emitf("  mov rdx, 0\n"); // Clear rdx for division (rdx:rax is dividend)
                    emitf("  idiv rbx\n"); // rax = (rdx:rax) / rbx
                    break;
                default:
                    fprintf(stderr, "Code Generation Error: Unknown binary operator.\n");
                    exit(1);
            }
            break;
        case AST_FUNCTION_CALL: {
            // Push arguments onto stack (right-to-left for cdecl-like behavior, or use registers for x64 ABI)
            // For simplicity, we'll use registers for first few args, then stack.
            // This example assumes System V AMD64 ABI (Linux/macOS)
            ASTNodeList* args_list = node->data.function_call.args->data.node_list.list;
            
            // Evaluate arguments and push them onto the stack in reverse order
            // Or, for x86-64 System V ABI, use registers for first 6 args.
            // This simplified example will push all args to stack for now for simplicity,
            // then move to registers if within first 6.
            // A more robust implementation would manage register allocation.
            
            // Save caller-saved registers if needed before function call (e.g., RAX, RCX, RDX, RSI, RDI, R8-R11)
            // For this simple example, we assume no complex register usage conflicts.

            int num_args = args_list->count;
            // Arguments beyond 6 are pushed onto the stack in reverse order.
            // Arguments 1-6 are passed in RDI, RSI, RDX, RCX, R8, R9.
            // We'll generate code that puts results into RAX and then moves them to appropriate regs.

            // Evaluate arguments and store them in temporary stack space or registers
            // For simplicity, let's assume max 6 arguments and place them directly into registers.
            // This is a very simplified approach for a beginner compiler.
            // A real compiler would manage stack frame and register allocation.
            
            // Push arguments onto stack for evaluation, then move to registers
            // This is a common pattern for handling expressions as arguments.
            for (int i = num_args - 1; i >= 0; --i) {
                generate_expression_code(args_list->nodes[i]); // Result in RAX
                emitf("  push rax\n"); // Push argument value
                current_stack_offset += 8;
            }

            // Pop arguments into registers in correct order (RDI, RSI, RDX, RCX, R8, R9)
            // This assumes integer arguments.
            const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
            for (int i = 0; i < num_args && i < 6; ++i) {
                emitf("  pop %s\n", arg_regs[i]);
                current_stack_offset -= 8;
            }

            // Align stack to 16-byte boundary before call if needed (System V ABI)
            // This is crucial for many C functions.
            int stack_adjustment = (current_stack_offset % 16!= 0)? (16 - (current_stack_offset % 16)) : 0;
            if (stack_adjustment > 0) {
                emitf("  sub rsp, %d\n", stack_adjustment);
                current_stack_offset += stack_adjustment;
            }

            emitf("  call %s\n", node->data.function_call.name); // Call the function

            // Clean up stack after call (if arguments were pushed beyond 6 registers)
            // For System V ABI, caller cleans up stack for arguments passed on stack.
            // Here, we pushed all args, so we pop them.
            // The `pop` operations above handle the cleanup for up to 6 args.
            // If more than 6 args were passed, they would be popped here.
            
            if (stack_adjustment > 0) {
                emitf("  add rsp, %d\n", stack_adjustment);
                current_stack_offset -= stack_adjustment;
            }

            // Return value is in RAX, as per convention
            break;
        }
        default:
            fprintf(stderr, "Code Generation Error: Unexpected AST node type in expression: %d\n", node->type);
            exit(1);
    }
}

// Function to generate code for statements
void generate_statement_code(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_RETURN_STMT:
            generate_expression_code(node->data.return_stmt.expr);
            // The return value is already in rax, which is the convention
            // Function epilogue will handle `ret` instruction
            break;
        case AST_EXPRESSION_STMT:
            generate_expression_code(node->data.expression_stmt.expr);
            // If it's just an expression statement, its result might be discarded
            break;
        case AST_BLOCK:
            for (size_t i = 0; i < node->data.node_list.list->count; ++i) {
                generate_statement_code(node->data.node_list.list->nodes[i]);
            }
            break;
        default:
            fprintf(stderr, "Code Generation Error: Unexpected AST node type in statement: %d\n", node->type);
            exit(1);
    }
}

// Main code generation function
void generate_code(ASTNode* ast) {
    if (!ast || ast->type!= AST_PROGRAM) {
        fprintf(stderr, "Code Generation Error: Invalid AST root node.\n");
        exit(1);
    }

    emitf(".intel_syntax noprefix\n"); // Use Intel syntax, no % prefix
    emitf(".data\n"); // Data section (if needed for global variables, not used here)
    emitf(".text\n"); // Code section

    // Iterate through function definitions
    for (size_t i = 0; i < ast->data.node_list.list->count; ++i) {
        ASTNode* func_def = ast->data.node_list.list->nodes[i];
        if (func_def->type!= AST_FUNCTION_DEF) {
            fprintf(stderr, "Code Generation Error: Expected function definition.\n");
            exit(1);
        }

        emitf(".global %s\n", func_def->data.function_def.name); // Declare global function
        emitf("%s:\n", func_def->data.function_def.name); // Function label

        // Function Prologue
        emitf("  push rbp\n"); // Save old base pointer [38, 39, 40]
        emitf("  mov rbp, rsp\n"); // Set new base pointer [38, 39, 40]
        
        // Allocate space for local variables (if any)
        // For this simple compiler, we assume no explicit local variable declarations yet.
        // If there were local variables, space would be allocated here: sub rsp, <size_of_locals>
        
        // Handle parameters: For System V ABI, first 6 integer parameters are in registers.
        // We need to store them on the stack if they are used as local variables.
        // For this simple compiler, we'll assume parameters are not directly used as local variables
        // and only function calls are handled, where arguments are passed via registers/stack.
        // A more complete compiler would iterate through func_def->data.function_def.params
        // and store them at specific offsets from RBP.
        
        // Example for storing parameters on stack (simplified, assuming all are 8-byte ints):
        // int param_offset = 16; // Start after RBP and return address
        // for (size_t p_idx = 0; p_idx < func_def->data.function_def.params->data.node_list.list->count; ++p_idx) {
        //     // This mapping needs to be consistent with calling convention
        //     // For System V ABI, RDI, RSI, RDX, RCX, R8, R9 are used for first 6.
        //     // Beyond that, parameters are on the stack.
        //     // This requires a symbol table to map parameter names to their stack/register locations.
        //     // For simplicity, this is omitted.
        // }

        // Generate code for function body
        generate_statement_code(func_def->data.function_def.body);

        // Function Epilogue
        emitf("  mov rsp, rbp\n"); // Restore stack pointer [38, 39, 40]
        emitf("  pop rbp\n"); // Restore old base pointer [38, 39, 40]
        emitf("  ret\n"); // Return from function [38, 40]
        emitf("\n");
    }
}