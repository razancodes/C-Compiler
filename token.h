#ifndef TOKEN_H
#define TOKEN_H
// header file
typedef enum{
    TOKEN_EOF = 0, // End of file
    TOKEN_NUMBER, //REST ALL NUMBERS ARE AUTOMATICALLY ASSUMED
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_LPAREN, // (
    TOKEN_RPAREN, // )
    TOKEN_IDENTIFIER,
    TOKEN_SEMICOLON, // ;
    TOKEN_LBRACE,    // {
    TOKEN_RBRACE,    // }
    TOKEN_RETURN,    // return keyword
    TOKEN_INT,       // int keyword
    TOKEN_COMMA,     // ,
    TOKEN_ASSIGN
}TokenType;

// a token has two attributes, type and class
// so we make a token structure having type and class
typedef struct{
    TokenType type; 
    union{ //uses memory for more efficient management
    int int_value; // classification of the token
    char* string_value; // actual text of the token
    }value;
}Token;
// we make two more functions create token to initialse a token and free_token to free up the memory
Token getNextToken();
#endif
