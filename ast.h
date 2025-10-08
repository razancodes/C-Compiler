#ifndef AST_H
#define AST_H

#include "token.h"
#include <stdlib.h>

// FORWARD DECLARING for recursive types
typedef struct ASTNode ASTNode;

//A generic list of ASTNodes, useful for blocks, argument lists, etc.
typedef struct ASTNodeList {
    ASTNode** nodes;
    size_t count;
    size_t capacity;
}ASTNodeList;

// Main AST Node structure (tagged union)
typedef enum{
    AST_PROGRAM,
    AST_FUNCTION_DEF,
    AST_PARAM_LIST,
    AST_BLOCK,
    AST_RETURN_STMT,
    AST_EXPRESSION_STMT,
    AST_NUMBER,
    AST_BINARY_OP,
    AST_IDENTIFIER,
    AST_FUNCTION_CALL,
    AST_ARG_LIST,
    // Add more node types as needed
}ASTNodeType;

struct ASTNode {
    ASTNodeType type; // Using the enum directly
    union {
        struct { int value; } number;
        struct { char* name; } identifier;
        struct { TokenType op; ASTNode* left; ASTNode* right; } binary_op;
        struct { char* name; ASTNode* params; ASTNode* body; } function_def; // params is AST_PARAM_LIST
        struct { char* name; ASTNode* args; } function_call; // args is AST_ARG_LIST
        struct { ASTNode* expr; } return_stmt;
        struct { ASTNode* expr; } expression_stmt;
        struct { ASTNodeList* list; } node_list; // For AST_PROGRAM, AST_PARAM_LIST, AST_BLOCK, AST_ARG_LIST
    } data;
};

// AST Node creation functions
ASTNode* ast_new_program(ASTNodeList* functions);
ASTNode* ast_new_function_def(char* name, ASTNode* params, ASTNode* body);
ASTNode* ast_new_param_list(ASTNodeList* params);
ASTNode* ast_new_block(ASTNodeList* statements);
ASTNode* ast_new_return_stmt(ASTNode* expr);
ASTNode* ast_new_expression_stmt(ASTNode* expr);
ASTNode* ast_new_number(int value);
ASTNode* ast_new_binary_op(TokenType op, ASTNode* left, ASTNode* right);
ASTNode* ast_new_identifier(char* name);
ASTNode* ast_new_function_call(char* name, ASTNode* args);
ASTNode* ast_new_arg_list(ASTNodeList* args);

// Helper for ASTNodeList
ASTNodeList* ast_new_node_list();
void ast_node_list_add(ASTNodeList* list, ASTNode* node);

// AST utility functions (e.g., printing, freeing)
void ast_print(ASTNode* node, int indent);
void ast_free(ASTNode* node);

#endif