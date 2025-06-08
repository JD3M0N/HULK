#pragma once
#include <string>
#include <vector>
#include "../token/Token.h"

// Declaraciones externas para interactuar con el lexer generado por flex
extern int yylex();
extern char* yytext;
extern int yylineno;
extern int yycolumn;
extern void yy_scan_string(const char* str);

class HULKLexer {
private:
    std::string sourceCode;
    
public:
    HULKLexer(const std::string& code) : sourceCode(code) {
        yy_scan_string(sourceCode.c_str());
    }
    
    // Método para obtener el siguiente token
    Token getNextToken();
    
    // Método para analizar todo el código
    std::vector<Token> tokenize();
};