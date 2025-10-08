// parser.c
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "parser.h"
#include "ast.h"

Token current_token;


void advance() {
    // Free previous string_value if it was dynamically allocated
    if (current_token.type == TOKEN_IDENTIFIER ||
        current_token.type == TOKEN_RETURN ||
        current_token.type == TOKEN_INT) { // Add other keywords if they allocate string_value
        if (current_token.value.string_value) {
            free(current_token.value.string_value);
            current_token.value.string_value = NULL; // Prevent double free
        }
    }
   current_token = getNextToken();
}
void match(TokenType expected_type){
    if (current_token.type==expected_type){
        advance();
    }
    else{
        fprintf(stderr,"Parser Error: Expected token type %d, got %d\n");
        exit(1);
    }
}
// parsing functions for arithmetic expressions:
ASTNode* parse_factor(){
    ASTNode* node = NULL;
    if (current_token.type == TOKEN_NUMBER){
        node =ast_new_number(current_token.value.int_value);
        match(TOKEN_NUMBER);
    }else if(current_token.type == TOKEN_IDENTIFIER){
        char* id_name = strdup(current_token.value.string_value);
        node = ast_new_identifier(id_name);
        match(TOKEN_IDENTIFIER);
        if (current_token.type==TOKEN_LPAREN){
            node = parse_function_call_from_id(node);
        }
    }
    else if(current_token.type==TOKEN_LPAREN){
        match(TOKEN_LPAREN);
        node = parse_expression();
        match(TOKEN_RPAREN);
        }
    else{
        fprintf(stderr,"Parser Error: Unexpected token in factor: %d\n",current_token.type);
        exit(1);
    }
    return node;
    }
ASTNode* parse_term(){
    ASTNode* left= parse_factor();
    while (current_token.type==TOKEN_MULTIPLY || current_token.type==TOKEN_DIVIDE){
        TokenType op_type=current_token.type;
        match(op_type);
        ASTNode* right = parse_factor();
        left =ast_new_binary_op(op_type,left,right);
    }
    return left;
}
ASTNode* parse_expression() {
    ASTNode* left = parse_term();
    while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS) {
        TokenType op_type = current_token.type;
        match(op_type);
        ASTNode* right = parse_term();
        left = ast_new_binary_op(op_type, left, right);
    }
    return left;
}
// Helper for function call parsing (called from parse_factor)
ASTNode* parse_function_call_from_id(ASTNode* id_node) {
    match(TOKEN_LPAREN);
    ASTNode* args = parse_argument_list(); // This will return an AST_ARG_LIST
    match(TOKEN_RPAREN);
    char* func_name = strdup(id_node->data.identifier.name); // Duplicate name from ID node
    ast_free(id_node); // Free the temporary ID node
    return ast_new_function_call(func_name, args);
}
// Parsing for argument list
ASTNode* parse_argument_list() {
    ASTNodeList* args_list = ast_new_node_list();
    if (current_token.type!= TOKEN_RPAREN) { // Check if there are arguments
        ast_node_list_add(args_list, parse_expression());
        while (current_token.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);
            ast_node_list_add(args_list, parse_expression());
        }
    }
    return ast_new_arg_list(args_list); // Wrap in an AST_ARG_LIST node
}
ASTNode* parse_statement() {
    if (current_token.type == TOKEN_RETURN) {
        match(TOKEN_RETURN);
        ASTNode* expr = parse_expression();
        match(TOKEN_SEMICOLON);
        return ast_new_return_stmt(expr);
    } else { // Assume it's an expression statement for now
        ASTNode* expr = parse_expression();
        match(TOKEN_SEMICOLON);
        return ast_new_expression_stmt(expr);
    }
}
ASTNode* parse_statement_list() {
    ASTNodeList* stmt_list = ast_new_node_list();
    while (current_token.type!= TOKEN_RBRACE && current_token.type!= TOKEN_EOF) {
        ast_node_list_add(stmt_list, parse_statement());
    }
    return ast_new_block(stmt_list); // Wrap in an AST_BLOCK node
}
ASTNode* parse_parameter_list() {
    ASTNodeList* param_list = ast_new_node_list();
    if (current_token.type == TOKEN_INT) { // Only 'int' type parameters for now
        match(TOKEN_INT);
        char* param_name = strdup(current_token.value.string_value); // Duplicate name
        ASTNode* param_id = ast_new_identifier(param_name);
        ast_node_list_add(param_list, param_id);
        match(TOKEN_IDENTIFIER);
        while (current_token.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);
            match(TOKEN_INT); // Only 'int' type parameters
            param_name = strdup(current_token.value.string_value); // Duplicate name
            param_id = ast_new_identifier(param_name);
            ast_node_list_add(param_list, param_id);
            match(TOKEN_IDENTIFIER);
        }
    }
    return ast_new_param_list(param_list); // Wrap in an AST_PARAM_LIST node
}

ASTNode* parse_function_definition() {
    match(TOKEN_INT); // Return type is always int for now
    char* func_name = strdup(current_token.value.string_value);
    match(TOKEN_IDENTIFIER);
    match(TOKEN_LPAREN);
    ASTNode* params = parse_parameter_list();
    match(TOKEN_RPAREN);
    match(TOKEN_LBRACE);
    ASTNode* body = parse_statement_list();
    match(TOKEN_RBRACE);
    return ast_new_function_def(func_name, params, body);
}

ASTNode* parse_program() {
    // lexer_init must be called externally before parse_program is called
    // advance() must be called externally once to get the first token
    ASTNodeList* func_list = ast_new_node_list();
    while (current_token.type!= TOKEN_EOF) {
        ast_node_list_add(func_list, parse_function_definition());
    }
    return ast_new_program(func_list); // Wrap in an AST_PROGRAM node
}
