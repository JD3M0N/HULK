#ifndef LEXER_ERROR_INTEGRATION_HPP
#define LEXER_ERROR_INTEGRATION_HPP

#include "../Error/error_handler.hpp"
#include "../Error/error_utils.hpp"

// Variables externas del lexer
extern int yylineno;
extern int yycolumn;
extern char *yytext;

// Función para reportar errores lexicales con información de ubicación
inline void reportLexicalError(const std::string &message, const std::string &filename = "")
{
    SourceLocation loc(filename, yylineno, yycolumn);
    std::string context = yytext ? std::string(yytext) : "";

    ErrorHandler::getInstance().lexicalError(message, loc, context);
}

// Función para reportar advertencias lexicales
inline void reportLexicalWarning(const std::string &message, const std::string &filename = "")
{
    SourceLocation loc(filename, yylineno, yycolumn);
    std::string context = yytext ? std::string(yytext) : "";

    ErrorHandler::getInstance().reportWarning(ErrorType::LEXICAL_ERROR, message, loc, context);
}

// Macro para uso en el lexer
#define LEX_ERROR(msg) reportLexicalError(msg, yyin ? "" : "")
#define LEX_WARNING(msg) reportLexicalWarning(msg, yyin ? "" : "")

#endif // LEXER_ERROR_INTEGRATION_HPP
