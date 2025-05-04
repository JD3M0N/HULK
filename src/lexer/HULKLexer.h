#ifndef HULKLEXER_H
#define HULKLEXER_H

#include "../token/Token.h"
#include <string>
#include <cctype>
#include <stdexcept>

class HULKLexer
{
public:
    explicit HULKLexer(const std::string &input);
    Token nextToken();

private:
    const std::string text;
    size_t pos = 0;

    void skipWhitespace();
    Token readNumber();
    Token readIdentifier();
};

#endif // HULKLEXER_H
