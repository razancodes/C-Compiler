#include "token.h"
#include <stdlib.h> // for malloc 
#include <string.h>
// allocates memory on the heap to the struct Token and then returns the address of its memory space
Token* create_token(TokenType type, const char* lexeme){
    Token* token = (Token*)malloc(sizeof(Token));
    if (token == NULL){ //pointer is null meaning we failed to allocate the memory
        perror("failed to allocate memory for the token");
        exit(EXIT_FAILURE);
    }
    token->type=type;
    token->lexeme=strdup(lexeme); //using strdup over strcpy coz that manages the memory as well
    if (token->lexeme==NULL){
        perror("Failed ot duplicate the string");
        free(token);
        exit(EXIT_FAILURE);
    }
    return token;
};
//freeing a token after its use is done.
void free_token(Token* token){
    if (token){
        free(token->lexeme); // first free the duplicated string
        free(token); // then duplicating the 
    }
};


