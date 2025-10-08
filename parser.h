#ifndef PARSER_H
#define parser_h

#include "token.h"
#include "ast.h"

//global token for current parser lookahead
extern Token current_token;

//function to move to next token
void advance();

//function to match an expected token type
void match(TokenType expected_type);


// Parsing functions for our grammar rules:
ASTNode* parse_program();
ASTNode* parse_function_definition();
ASTNode* parse_parameter_list(); // Returns an AST_PARAM_LIST node
ASTNode* parse_statement_list();// Returns an AST_BLOCK node
ASTNode* parse_statement();
ASTNode* parse_expression();
ASTNode* parse_term();
ASTNode* parse_factor();
ASTNode* parse_function_call_from_id(ASTNode* id_node); // Helper for function calls
ASTNode* parse_argument_list(); // Returns an AST_ARG_LIST node

// grammar for c lang: 
/* program    -> function_definition+
function_definition -> TOKEN_INT TOKEN_IDENTIFIER TOKEN_LPAREN parameter_list TOKEN_RPAREN TOKEN_LBRACE statement_list TOKEN_RBRACE
parameter_list -> (TOKEN_INT TOKEN_IDENTIFIER (TOKEN_COMMA TOKEN_INT TOKEN_IDENTIFIER)*)?
statement_list -> statement*
statement  -> TOKEN_RETURN expression TOKEN_SEMICOLON
| expression TOKEN_SEMICOLON
expression -> term ( (TOKEN_PLUS | TOKEN_MINUS) term )*
term       -> factor ( (TOKEN_MULTIPLY | TOKEN_DIVIDE) factor )*
factor     -> TOKEN_NUMBER
| TOKEN_IDENTIFIER (TOKEN_LPAREN argument_list TOKEN_RPAREN)? // Handles identifiers and function calls
| TOKEN_LPAREN expression TOKEN_RPAREN
argument_list -> (expression (TOKEN_COMMA expression)*)? */

#endif