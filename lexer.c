
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isalnum, isdigit, isalpha, isspace

#define MAX_ID_LEN 64
static const char* input_ptr;
Token getNextToken();

static void skip_whitespace_and_comments(){
    while (*input_ptr!='\0'){
        if (isspace(*input_ptr)){  //skipping whitespace
            input_ptr++;
        }
        else if (*input_ptr == '/' && *(input_ptr + 1) == '/'){
            input_ptr+=2; // skip single line comment
            while (*input_ptr!='\n' && *input_ptr != '\0'){
                input_ptr++;
            }
        }
        else if(*input_ptr == '/' && *(input_ptr + 1) == '*') { // Multi-line comment
            input_ptr += 2;
            while (!(*input_ptr == '*' && *(input_ptr + 1) == '/') && *input_ptr!= '\0') {
                input_ptr++;
            if (*input_ptr!='\0'){ // skips the closing sequence 
                input_ptr+=2;
            }
        }
        }
        else{
            break; // Not whitespace or comment
        }
    }
}


static Token create_token(TokenType type){
    Token t;
    t.type=type;
    return t;
};
static Token create_number_token(int value) {
    Token t = create_token(TOKEN_NUMBER);
    t.value.int_value = value;
    return t;
}
static Token create_string_token(TokenType type, const char* str_value){
    Token t = create_token(type);
    t.value.string_value=strdup(str_value);
    return t;
}
Token getNextToken(){
    skip_whitespace_and_comments();
    if (*input_ptr=='\0'){
        return create_token(TOKEN_EOF);
    }
    // Handle numbers:
    if (isdigit(*input_ptr)){
        int value = 0;
        while (isdigit(*input_ptr)){
            value=value*10+ (*input_ptr-'0'); // method to convert a string digit to an int
            input_ptr++;
        }
        return create_number_token(value);
    };
    switch(*input_ptr){
        case '+': input_ptr++; return create_token(TOKEN_PLUS);
        case '-':input_ptr++; return create_token(TOKEN_MINUS);
        case '*': input_ptr++; return create_token(TOKEN_MULTIPLY);
        case '/': input_ptr++; return create_token(TOKEN_DIVIDE);
        case '(': input_ptr++; return create_token(TOKEN_LPAREN);
        case ')': input_ptr++; return create_token(TOKEN_RPAREN);
        case ';': input_ptr++; return create_token(TOKEN_SEMICOLON);
        case '{': input_ptr++; return create_token(TOKEN_LBRACE);
        case '}': input_ptr++; return create_token(TOKEN_RBRACE);
        case ',': input_ptr++; return create_token(TOKEN_COMMA);
        case '=': input_ptr++; return create_token(TOKEN_ASSIGN);
    }
    // handle identifiers and keywords: 
    if (isalpha(*input_ptr) || *input_ptr == '_') {
        char buffer[MAX_ID_LEN]; // Max identifier length
        int i = 0;
        while (((isalnum(*input_ptr) || *input_ptr == '_')) && (i<MAX_ID_LEN-1)) {
            buffer[i++] = *input_ptr++;
        }
        buffer[i] = '\0';

        // Check for keywords
        if (strcmp(buffer, "return") == 0) {
            return create_string_token(TOKEN_RETURN, buffer);
        } else if (strcmp(buffer, "int") == 0) {
            return create_string_token(TOKEN_INT, buffer);
        }
        // Add other keywords here as the language expands
        return create_string_token(TOKEN_IDENTIFIER, buffer);
    }
    //UNKNOWN CHARACTER 
    fprintf(stderr, "Lexer Error: Unknown character '%c'\n", *input_ptr);
    input_ptr++; // Advance to avoid infinite loop
    return create_token(TOKEN_EOF);}
 // Or a specific error token

void lexer_init(const char* source_code){
    input_ptr = source_code;
}