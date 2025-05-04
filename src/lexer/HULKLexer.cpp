#include "HULKLexer.h"
#include <iostream>

HULKLexer::HULKLexer(const std::string &input)
    : text(input), pos(0)
{
    std::cerr << "[Lexer] Initialized with input length=" << text.size() << "\n";
}

void HULKLexer::skipWhitespace()
{
    while (pos < text.size() && std::isspace(text[pos]))
        ++pos;
}

Token HULKLexer::readNumber()
{
    size_t start = pos;
    while (pos < text.size() && (std::isdigit(text[pos]) || text[pos] == '.'))
        ++pos;
    double val = std::stod(text.substr(start, pos - start));
    Token tok(TokenType::NUM, "", val);
    std::cerr << "[Lexer] readNumber -> " << val << "\n";
    return tok;
}

Token HULKLexer::readIdentifier()
{
    size_t start = pos;
    while (pos < text.size() && std::isalpha(text[pos]))
        ++pos;
    std::string id = text.substr(start, pos - start);
    Token tok(TokenType::ID, id, 0);
    std::cerr << "[Lexer] readIdentifier -> " << id << "\n";
    return tok;
}

Token HULKLexer::nextToken()
{
    skipWhitespace();
    if (pos >= text.size())
    {
        std::cerr << "[Lexer] nextToken -> EOF\n";
        return Token(TokenType::EOF_TOK);
    }

    char c = text[pos++];
    // Inicializamos tok con un valor válido; luego lo reasignamos en el switch
    Token tok(TokenType::EOF_TOK);

    switch (c)
    {
    case '+':
        tok = Token(TokenType::PLUS, "+");
        break;
    case '-':
        tok = Token(TokenType::MINUS, "-");
        break;
    case '*':
        tok = Token(TokenType::STAR, "*");
        break;
    case '/':
        tok = Token(TokenType::SLASH, "/");
        break;
    case '(':
        tok = Token(TokenType::OPAR, "(");
        break;
    case ')':
        tok = Token(TokenType::CPAR, ")");
        break;
    default:
        if (std::isdigit(c))
        {
            --pos;
            tok = readNumber();
        }
        else if (std::isalpha(c))
        {
            --pos;
            tok = readIdentifier();
        }
        else
        {
            throw std::runtime_error(std::string("Unexpected char: ") + c);
        }
    }

    std::cerr << "[Lexer] nextToken -> type=" << static_cast<int>(tok.type)
              << " lexeme='" << tok.lexeme << "' value=" << tok.value << "\n";
    return tok;
}
