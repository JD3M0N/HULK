#include "HULKLexer.h"
#include "../error_handler.hpp"
#include "../SintacticoBison.tab.hpp"
#include <iostream>

// Declaraciones externas de funciones generadas por Flex
extern int yylex();
extern char* yytext;
extern int yylineno;
extern int yycolumn;
extern void yy_scan_string(const char* str);

HULKLexer::HULKLexer(const std::string& code) : sourceCode(code) {
    // Configurar flex para leer desde la cadena de código
    yy_scan_string(sourceCode.c_str());
}

Token HULKLexer::getNextToken() {
    int tokenCode = yylex();
    
    if (tokenCode == 0) {
        return Token(TokenType::END_OF_FILE, yylineno, yycolumn);
    }
    
    TokenLocation location(yylineno, yycolumn - strlen(yytext));
    
    switch (tokenCode) {
        case NUMBER:
            return Token(TokenType::NUMBER, atof(yytext), location.line, location.column);
        
        case ID:
            return Token(TokenType::IDENTIFIER, std::string(yytext), location.line, location.column);
        
        case STRING:
            {
                std::string str(yytext + 1);  // Skip first quote
                str = str.substr(0, str.length() - 1);  // Remove last quote
                return Token(TokenType::STRING, str, location.line, location.column);
            }
        
        case '+': return Token(TokenType::PLUS, location.line, location.column);
        case '-': return Token(TokenType::MINUS, location.line, location.column);
        case '*': return Token(TokenType::MULTIPLY, location.line, location.column);
        case '/': return Token(TokenType::DIVIDE, location.line, location.column);
        case '^': return Token(TokenType::POWER, location.line, location.column);
        case '%': return Token(TokenType::MOD, location.line, location.column);
        case CONCAT: return Token(TokenType::CONCAT, location.line, location.column);
        
        case EQ: return Token(TokenType::EQUAL, location.line, location.column);
        case NEQ: return Token(TokenType::NOT_EQUAL, location.line, location.column);
        case '<': return Token(TokenType::LESS, location.line, location.column);
        case '>': return Token(TokenType::GREATER, location.line, location.column);
        case LE: return Token(TokenType::LESS_EQUAL, location.line, location.column);
        case GE: return Token(TokenType::GREATER_EQUAL, location.line, location.column);
        
        case AND: return Token(TokenType::AND, location.line, location.column);
        case OR: return Token(TokenType::OR, location.line, location.column);
        
        case ASSIGN: return Token(TokenType::ASSIGN, location.line, location.column);
        
        case '(': return Token(TokenType::LPAREN, location.line, location.column);
        case ')': return Token(TokenType::RPAREN, location.line, location.column);
        case '{': return Token(TokenType::LBRACE, location.line, location.column);
        case '}': return Token(TokenType::RBRACE, location.line, location.column);
        case ',': return Token(TokenType::COMMA, location.line, location.column);
        case ';': return Token(TokenType::SEMICOLON, location.line, location.column);
        
        case LET: return Token(TokenType::LET, location.line, location.column);
        case IN: return Token(TokenType::IN, location.line, location.column);
        case PRINT: return Token(TokenType::PRINT, location.line, location.column);
        case FUNCTION: return Token(TokenType::FUNCTION, location.line, location.column);
        case IF: return Token(TokenType::IF, location.line, location.column);
        case ELSE: return Token(TokenType::ELSE, location.line, location.column);
        case ELIF: return Token(TokenType::ELIF, location.line, location.column);
        case WHILE: return Token(TokenType::WHILE, location.line, location.column);
        case FOR: return Token(TokenType::FOR, location.line, location.column);
        case ARROW: return Token(TokenType::ARROW, location.line, location.column);
        
        default:
            reportLexicalError("Token desconocido: " + std::string(yytext), SourceLocation(location.line, location.column));
            return Token(TokenType::END_OF_FILE);
    }
}

std::vector<Token> HULKLexer::tokenize() {
    std::vector<Token> tokens;
    Token token = getNextToken();
    
    while (token.getType() != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = getNextToken();
    }
    
    tokens.push_back(token);  // Agregar token EOF
    return tokens;
}