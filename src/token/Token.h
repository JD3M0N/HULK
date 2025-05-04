#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// Enumeración con todos los tipos de token
enum class TokenType
{
    EOF_TOK,
    NUM,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    OPAR,
    CPAR,
    ID
};

// Clase Token independiente
class Token
{
public:
    TokenType type;
    std::string lexeme;
    double value; // solo se usa para NUM

    Token(TokenType t, const std::string &l = "", double v = 0)
        : type(t), lexeme(l), value(v) {}
};

#endif // TOKEN_H
