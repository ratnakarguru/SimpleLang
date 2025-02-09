#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Define the maximum token length
#define MAX_TOKEN_LEN 100

// Enumeration for different token types
typedef enum {
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_IF, TOKEN_EQUAL, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_SEMICOLON, TOKEN_UNKNOWN, TOKEN_EOF
} TokenType;

// Structure to represent a token
typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LEN];
} Token;

// Structure to represent a node in the Abstract Syntax Tree (AST)
typedef struct ASTNode {
    char *type;
    char *value;
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

// Lexer function to get the next token from the input file
void getNextToken(FILE *file, Token *token) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (isspace(c)) continue;
        if (isalpha(c)) {
            // Tokenize keywords and identifiers
            int len = 0;
            token->text[len++] = c;
            while (isalnum(c = fgetc(file))) {
                if (len < MAX_TOKEN_LEN - 1) token->text[len++] = c;
            }
            ungetc(c, file);
            token->text[len] = '\0';
            if (strcmp(token->text, "int") == 0) token->type = TOKEN_INT;
            else if (strcmp(token->text, "if") == 0) token->type = TOKEN_IF;
            else token->type = TOKEN_IDENTIFIER;
            return;
        }
        if (isdigit(c)) {
            // Tokenize numbers
            int len = 0;
            token->text[len++] = c;
            while (isdigit(c = fgetc(file))) {
                if (len < MAX_TOKEN_LEN - 1) token->text[len++] = c;
            }
            ungetc(c, file);
            token->text[len] = '\0';
            token->type = TOKEN_NUMBER;
            return;
        }
        // Tokenize operators and punctuation
        switch (c) {
            case '=': token->type = TOKEN_ASSIGN; token->text[0] = '='; token->text[1] = '\0'; return;
            case '+': token->type = TOKEN_PLUS; token->text[0] = '+'; token->text[1] = '\0'; return;
            case '-': token->type = TOKEN_MINUS; token->text[0] = '-'; token->text[1] = '\0'; return;
            case '(': token->type = TOKEN_LBRACE; token->text[0] = '('; token->text[1] = '\0'; return;
            case ')': token->type = TOKEN_RBRACE; token->text[0] = ')'; token->text[1] = '\0'; return;
            case ';': token->type = TOKEN_SEMICOLON; token->text[0] = ';'; token->text[1] = '\0'; return;
        }
    }
    // End of file token
    token->type = TOKEN_EOF;
    token->text[0] = '\0';
}

// Function to create a new AST node
ASTNode* createNode(char *type, char *value, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode*) malloc(sizeof(ASTNode));
    node->type = strdup(type);
    node->value = strdup(value);
    node->left = left;
    node->right = right;
    return node;
}

// Function prototypes for parsing
ASTNode* parseExpression(Token *tokens, int *index);
ASTNode* parseTerm(Token *tokens, int *index);
ASTNode* parseFactor(Token *tokens, int *index);

// Parse an expression (handle + and - operators)
ASTNode* parseExpression(Token *tokens, int *index) {
    ASTNode *left = parseTerm(tokens, index);
    while (tokens[*index].type == TOKEN_PLUS || tokens[*index].type == TOKEN_MINUS) {
        char *op = strdup(tokens[*index].text);
        (*index)++;
        ASTNode *right = parseTerm(tokens, index);
        left = createNode("operator", op, left, right);
    }
    return left;
}

// Parse a term (for this example, it directly calls parseFactor)
ASTNode* parseTerm(Token *tokens, int *index) {
    return parseFactor(tokens, index);
}

// Parse a factor (handle numbers and identifiers)
ASTNode* parseFactor(Token *tokens, int *index) {
    if (tokens[*index].type == TOKEN_NUMBER || tokens[*index].type == TOKEN_IDENTIFIER) {
        char *value = strdup(tokens[*index].text);
        (*index)++;
        return createNode("literal", value, NULL, NULL);
    }
    return NULL;
}

// Generate assembly code from the AST
void generateAssembly(ASTNode *node) {
    if (node == NULL) return;
    generateAssembly(node->left);
    printf("Generated assembly for: %s\n", node->value);
    generateAssembly(node->right);
}

// Print the AST in an in-order traversal
void printAST(ASTNode *node) {
    if (node == NULL) return;
    printAST(node->left);
    printf("%s ", node->value);
    printAST(node->right);
}

// Main function
int main() {
    // Open the input file
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }
    // Tokenize the input
    Token tokens[100];
    int index = 0;
    Token token;
    do {
        getNextToken(file, &token);
        tokens[index++] = token;
    } while (token.type != TOKEN_EOF);
    fclose(file);

    // Parse the tokens into an AST
    index = 0;
    ASTNode *ast = parseExpression(tokens, &index);

    // Print the AST
    printAST(ast);
    // Generate assembly from the AST
    generateAssembly(ast);

    return 0;
}
