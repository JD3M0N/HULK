#include "HULKParser.h"
#include "../error_handler.hpp"

// Declaración externa del analizador sintáctico generado por Bison
extern int yyparse();
extern Node* root;
extern void yy_scan_string(const char* str);

HULKParser::HULKParser(HULKLexer& lex) : lexer(lex) {
    // No es necesario hacer nada aquí ya que lexer ya está configurado
}

ASTPtr HULKParser::parse() {
    // Iniciar el análisis sintáctico
    int result = yyparse();
    
    if (result != 0) {
        // Error durante el análisis sintáctico
        throw SyntaxError("Error durante el análisis sintáctico");
    }
    
    if (root == nullptr) {
        throw SyntaxError("No se pudo construir el AST");
    }
    
    return root;
}