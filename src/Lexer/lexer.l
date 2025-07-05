%option noyywrap
%option noinput nounput

%{
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include <string>

#include "windows_compat.hpp"

// Función auxiliar para duplicar cadenas de forma portable
static char* hulk_strdup(const char* s) {
    if (!s) return nullptr;
    char* d = (char*)malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

#include "../AST/ast.hpp"
#include "../Parser/parser.tab.hpp"

extern int yylineno;
int yycolumn = 1;

// Estructura para almacenar la ubicación - reemplaza el uso de yylloc
typedef struct {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} TokenLocation;

TokenLocation current_location;

// Macro para actualizar la columna
#define YY_USER_ACTION \
    { \
        current_location.first_line = current_location.last_line = yylineno; \
        current_location.first_column = yycolumn; \
        current_location.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng; \
    }
%}

%%

\n                          { yylineno++; yycolumn = 1; }
[ \t\r]+                    { /* skip whitespace, but update column */ }

[0-9]+(\.[0-9]+)?           {
                              yylval.expr = new NumberExpr(atof(yytext));
                              return NUMBER;
                           }

\"([^\"]*)\"                {
                              std::string raw = yytext;
                              // Eliminar comillas del principio y final
                              std::string content = raw.substr(1, raw.length() - 2);
                              yylval.expr = new StringExpr(content);
                              return STRING;
                           }

[A-Za-z][A-Za-z0-9_]*  {
    if (strcmp(yytext, "if") == 0)       return IF;
    if (strcmp(yytext, "else") == 0)     return ELSE;
    if (strcmp(yytext, "elif") == 0)     return ELIF;
    if (strcmp(yytext, "let") == 0)      return LET;
    if (strcmp(yytext, "in") == 0)       return IN;
    if (strcmp(yytext, "while") == 0)    return WHILE;
    if (strcmp(yytext, "for") == 0)      return FOR;
    if (strcmp(yytext, "true") == 0)     { yylval.expr = new BooleanExpr(true);  return TRUE; }
    if (strcmp(yytext, "false") == 0)    { yylval.expr = new BooleanExpr(false); return FALSE; }
    if (strcmp(yytext,"function") == 0)  return FUNCTION; 
    if (strcmp(yytext, "type")     == 0) return TYPE;
    if (strcmp(yytext, "inherits") == 0) return INHERITS;
    if (strcmp(yytext, "new")      == 0) return NEW;
    if (strcmp(yytext, "self")     == 0) { yylval.str = hulk_strdup("self"); return SELF; }
    if (strcmp(yytext, "base")     == 0) { yylval.str = hulk_strdup("base"); return BASE; }    


    // Si no es ninguna palabra reservada, entonces es IDENT
    yylval.str = hulk_strdup(yytext);
    return IDENT;
}


"\."                        { return DOT; }                    
\,                          { return COMMA; }
\+                          { return PLUS; }
\-                          { return MINUS; }
\*                          { return MULT; }
\/                          { return DIV; }
\**                         { return POW; }
\(                          { return LPAREN; }
\)                          { return RPAREN; }
\;                          { return SEMICOLON; }
\%                          { return MOD; }
"<="                        { return LE; }
">="                        { return GE; }
"=="                        { return EQ; }
\!=                         { return NEQ; }
"<"                         { return LESS_THAN; }
">"                         { return GREATER_THAN; }
"||"                        { return OR; }
"&&"                        { return AND; }  
"="                         { return ASSIGN; }
\{                          { return LBRACE; }
\}                          { return RBRACE; }
":="                        { return ASSIGN_DESTRUCT; }
"=>"                        { return ARROW; }
"@"                         { return CONCAT; }
.                           {
                              fprintf(stderr, "Caracter no reconocido: %s en la línea %d\n", yytext, yylineno);
                           }

%%