#pragma once
#include "../lexer/HULKLexer.h"
#include "../tree.hpp"

// Declaraciones externas para interactuar con el parser generado por bison
extern int yyparse();
extern Node* root;

class HULKParser {
private:
    HULKLexer& lexer;
    
public:
    HULKParser(HULKLexer& lex) : lexer(lex) {}
    
    // Método principal para parsear el código
    Node* parse() {
        int result = yyparse();
        if (result != 0) {
            throw std::runtime_error("Error al analizar el código");
        }
        return root;
    }
};