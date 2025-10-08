#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static ASTNode* create_ast_node(ASTNodeType type){
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node){
        fprintf(stderr,"Memory allocation failed for ASTNode \n");
        exit(1);
    }
    node->type=type;
    return node;
};
ASTNodeList* ast_new_node_list(){
    ASTNodeList* list = (ASTNodeList*)malloc(sizeof(ASTNodeList));
    if (!list)(fprintf(stderr,"Memory allocation failed for ASTNodelist"));
    list->nodes=NULL;
    list->count=0;
    list->capacity=0;
    return list;
}
void ast_node_list_add(ASTNodeList* list, ASTNode* node){
    if (list->count>=list->capacity){
        list->capacity=list->capacity==0? 4:list->capacity*2;
        list->nodes=(ASTNode**)realloc(list->nodes,list->capacity*sizeof(ASTNode*));
        if(!list->nodes){(fprintf(stderr,"memory reallocation failed ASTNodelist nodes.\n")); exit(1);}
    }
    list->nodes[list->count++]=node;
}
// AST Node creation functions
ASTNode* ast_new_program(ASTNodeList* functions) {
    ASTNode* node = create_ast_node(AST_PROGRAM);
    node->data.node_list.list = functions;
    return node;
}

ASTNode* ast_new_function_def(char* name, ASTNode* params, ASTNode* body) {
    ASTNode* node = create_ast_node(AST_FUNCTION_DEF);
    node->data.function_def.name = strdup(name);
    node->data.function_def.params = params;
    node->data.function_def.body = body;
    return node;
}

ASTNode* ast_new_param_list(ASTNodeList* params) {
    ASTNode* node = create_ast_node(AST_PARAM_LIST);
    node->data.node_list.list = params;
    return node;
}

ASTNode* ast_new_block(ASTNodeList* statements) {
    ASTNode* node = create_ast_node(AST_BLOCK);
    node->data.node_list.list = statements;
    return node;
}

ASTNode* ast_new_return_stmt(ASTNode* expr) {
    ASTNode* node = create_ast_node(AST_RETURN_STMT);
    node->data.return_stmt.expr = expr;
    return node;
}

ASTNode* ast_new_expression_stmt(ASTNode* expr) {
    ASTNode* node = create_ast_node(AST_EXPRESSION_STMT);
    node->data.expression_stmt.expr = expr;
    return node;
}

ASTNode* ast_new_number(int value) {
    ASTNode* node = create_ast_node(AST_NUMBER);
    node->data.number.value = value;
    return node;
}

ASTNode* ast_new_binary_op(TokenType op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_ast_node(AST_BINARY_OP);
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

ASTNode* ast_new_identifier(char* name) {
    ASTNode* node = create_ast_node(AST_IDENTIFIER);
    node->data.identifier.name = strdup(name);
    return node;
}

ASTNode* ast_new_function_call(char* name, ASTNode* args) {
    ASTNode* node = create_ast_node(AST_FUNCTION_CALL);
    node->data.function_call.name = strdup(name);
    node->data.function_call.args = args;
    return node;
}

ASTNode* ast_new_arg_list(ASTNodeList* args) {
    ASTNode* node = create_ast_node(AST_ARG_LIST);
    node->data.node_list.list = args;
    return node;
}
// Basic AST printing (for debugging)
void ast_print_indent(int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("  ");
    }
}
void ast_print(ASTNode* node, int indent) {
    if (!node) return;

    ast_print_indent(indent);
    switch (node->type) {
        case AST_PROGRAM:
            printf("PROGRAM:\n");
            for (size_t i = 0; i < node->data.node_list.list->count; ++i) {
                ast_print(node->data.node_list.list->nodes[i], indent + 1);
            }
            break;
        case AST_FUNCTION_DEF:
            printf("FUNCTION_DEF: %s\n", node->data.function_def.name);
            ast_print_indent(indent + 1); printf("Parameters:\n");
            ast_print(node->data.function_def.params, indent + 2);
            ast_print_indent(indent + 1); printf("Body:\n");
            ast_print(node->data.function_def.body, indent + 2);
            break;
        case AST_PARAM_LIST:
        case AST_ARG_LIST:
        case AST_BLOCK:
            printf("%s_LIST (count: %zu):\n", node->type == AST_PARAM_LIST? "PARAM" : (node->type == AST_ARG_LIST? "ARG" : "BLOCK"), node->data.node_list.list->count);
            for (size_t i = 0; i < node->data.node_list.list->count; ++i) {
                ast_print(node->data.node_list.list->nodes[i], indent + 1);
            }
            break;
        case AST_RETURN_STMT:
            printf("RETURN_STMT:\n");
            ast_print(node->data.return_stmt.expr, indent + 1);
            break;
        case AST_EXPRESSION_STMT:
            printf("EXPRESSION_STMT:\n");
            ast_print(node->data.expression_stmt.expr, indent + 1);
            break;
        case AST_NUMBER:
            printf("NUMBER: %d\n", node->data.number.value);
            break;
        case AST_BINARY_OP:
            printf("BINARY_OP: %c\n",
                   node->data.binary_op.op == TOKEN_PLUS? '+' :
                   node->data.binary_op.op == TOKEN_MINUS? '-' :
                   node->data.binary_op.op == TOKEN_MULTIPLY? '*' : '/');
            ast_print(node->data.binary_op.left, indent + 1);
            ast_print(node->data.binary_op.right, indent + 1);
            break;
        case AST_IDENTIFIER:
            printf("IDENTIFIER: %s\n", node->data.identifier.name);
            break;
        case AST_FUNCTION_CALL:
            printf("FUNCTION_CALL: %s\n", node->data.function_call.name);
            ast_print_indent(indent + 1); printf("Arguments:\n");
            ast_print(node->data.function_call.args, indent + 2);
            break;
        default:
            printf("UNKNOWN_AST_NODE_TYPE: %d\n", node->type);
            break;
    }
}

// Basic AST freeing
void ast_free(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case AST_PROGRAM:
        case AST_PARAM_LIST:
        case AST_ARG_LIST:
        case AST_BLOCK:
            if (node->data.node_list.list) {
                for (size_t i = 0; i < node->data.node_list.list->count; ++i) {
                    ast_free(node->data.node_list.list->nodes[i]);
                }
                free(node->data.node_list.list->nodes);
                free(node->data.node_list.list);
            }
            break;
        case AST_FUNCTION_DEF:
            free(node->data.function_def.name);
            ast_free(node->data.function_def.params);
            ast_free(node->data.function_def.body);
            break;
        case AST_RETURN_STMT:
            ast_free(node->data.return_stmt.expr);
            break;
        case AST_EXPRESSION_STMT:
            ast_free(node->data.expression_stmt.expr);
            break;
        case AST_BINARY_OP:
            ast_free(node->data.binary_op.left);
            ast_free(node->data.binary_op.right);
            break;
        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;
        case AST_FUNCTION_CALL:
            free(node->data.function_call.name);
            ast_free(node->data.function_call.args);
            break;
        case AST_NUMBER:
            // No dynamic memory to free for numbers
            break;
        default:
            break;
    }
    free(node);
}