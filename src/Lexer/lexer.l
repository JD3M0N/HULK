%option noyywrap
%option noinput nounput

%{
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include <string>

#include "../AST/ast.hpp"
#include "../Parser/parser.tab.hpp"

extern int yylineno;

// Necesario para evitar conflictos con strdup (definido en <string.h>)
#ifdef strdup
#undef strdup
#endif
%}

%%

\n                          { yylineno++; }
[ \t\r]+                    { /* skip whitespace */ }

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
    if (strcmp(yytext, "self")     == 0) { yylval.str = strdup("self"); return SELF; }
    if (strcmp(yytext, "base")     == 0) { yylval.str = strdup("base"); return BASE; }    


    // Si no es ninguna palabra reservada, entonces es IDENT
    yylval.str = strdup(yytext);
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