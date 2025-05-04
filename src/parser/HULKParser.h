#ifndef HULKPARSER_H
#define HULKPARSER_H

#include "../lexer/HULKLexer.h"
#include "../AST/AST.h"

class HULKParser
{
public:
    explicit HULKParser(HULKLexer &lex);
    ASTPtr parse();

private:
    HULKLexer &lexer;
    Token lookahead;

    void consume(TokenType);
    ASTPtr parseExpression();
    ASTPtr parseExpressionPrime(ASTPtr left);
    ASTPtr parseTerm();
    ASTPtr parseTermPrime(ASTPtr left);
    ASTPtr parseFactor();
};

#endif // HULKPARSER_H
