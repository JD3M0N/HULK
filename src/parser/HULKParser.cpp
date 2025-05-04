#include "HULKParser.h"
#include <iostream>

HULKParser::HULKParser(HULKLexer &lex)
    : lexer(lex), lookahead(lexer.nextToken())
{
    std::cerr << "[Parser] Initialized, first lookahead='"
              << lookahead.lexeme << "'\n";
}

void HULKParser::consume(TokenType t)
{
    std::cerr << "[Parser] consume, expected=" << static_cast<int>(t)
              << " got=" << static_cast<int>(lookahead.type) << "\n";
    if (lookahead.type == t)
    {
        lookahead = lexer.nextToken();
        std::cerr << "[Parser] new lookahead='" << lookahead.lexeme << "'\n";
    }
    else
    {
        throw std::runtime_error("Token inesperado en el parser");
    }
}

ASTPtr HULKParser::parse()
{
    std::cerr << "[Parser] Enter parse()\n";
    ASTPtr root = parseExpression();
    consume(TokenType::EOF_TOK);
    std::cerr << "[Parser] Exit parse()\n";
    return root;
}

ASTPtr HULKParser::parseExpression()
{
    std::cerr << "[Parser] Enter parseExpression, lookahead='"
              << lookahead.lexeme << "'\n";
    ASTPtr left = parseTerm();
    ASTPtr res = parseExpressionPrime(left);
    std::cerr << "[Parser] Exit parseExpression\n";
    return res;
}

ASTPtr HULKParser::parseExpressionPrime(ASTPtr left)
{
    std::cerr << "[Parser] Enter parseExpressionPrime, left-node\n";
    if (lookahead.type == TokenType::PLUS)
    {
        consume(TokenType::PLUS);
        ASTPtr right = parseTerm();
        ASTPtr node = new BinaryOpNode('+', left, right);
        return parseExpressionPrime(node);
    }
    else if (lookahead.type == TokenType::MINUS)
    {
        consume(TokenType::MINUS);
        ASTPtr right = parseTerm();
        ASTPtr node = new BinaryOpNode('-', left, right);
        return parseExpressionPrime(node);
    }
    else
    {
        std::cerr << "[Parser] Exit parseExpressionPrime (ε)\n";
        return left;
    }
}

ASTPtr HULKParser::parseTerm()
{
    std::cerr << "[Parser] Enter parseTerm\n";
    ASTPtr left = parseFactor();
    ASTPtr res = parseTermPrime(left);
    std::cerr << "[Parser] Exit parseTerm\n";
    return res;
}

ASTPtr HULKParser::parseTermPrime(ASTPtr left)
{
    std::cerr << "[Parser] Enter parseTermPrime\n";
    if (lookahead.type == TokenType::STAR)
    {
        consume(TokenType::STAR);
        ASTPtr right = parseFactor();
        ASTPtr node = new BinaryOpNode('*', left, right);
        return parseTermPrime(node);
    }
    else if (lookahead.type == TokenType::SLASH)
    {
        consume(TokenType::SLASH);
        ASTPtr right = parseFactor();
        ASTPtr node = new BinaryOpNode('/', left, right);
        return parseTermPrime(node);
    }
    else
    {
        std::cerr << "[Parser] Exit parseTermPrime (ε)\n";
        return left;
    }
}

ASTPtr HULKParser::parseFactor()
{
    std::cerr << "[Parser] Enter parseFactor, lookahead='"
              << lookahead.lexeme << "'\n";
    if (lookahead.type == TokenType::NUM)
    {
        double val = lookahead.value;
        consume(TokenType::NUM);
        std::cerr << "[Parser] NumberNode(" << val << ")\n";
        return new NumberNode(val);
    }
    else if (lookahead.type == TokenType::OPAR)
    {
        consume(TokenType::OPAR);
        ASTPtr node = parseExpression();
        consume(TokenType::CPAR);
        return node;
    }
    else if (lookahead.type == TokenType::ID)
    {
        std::string name = lookahead.lexeme;
        consume(TokenType::ID);
        consume(TokenType::OPAR);
        ASTPtr arg = parseExpression();
        consume(TokenType::CPAR);
        std::cerr << "[Parser] FunctionCallNode(" << name << ")\n";
        return new FunctionCallNode(name, arg);
    }
    else
    {
        throw std::runtime_error("Factor mal formado");
    }
}
