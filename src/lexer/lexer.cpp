#include "Lexer.h"
#include <cctype>

// Constructor
Lexer::Lexer(const std::string &source) : source(source) {}

// Verifica si se ha llegado al final del string
bool Lexer::isAtEnd() const
{
    return current >= source.length();
}

// Avanza un caracter y lo retorna
char Lexer::advance()
{
    return source[current++];
}

// Mira el caracter actual sin avanzar
char Lexer::peek() const
{
    if (isAtEnd())
        return '\0';
    return source[current];
}

// Mira el siguiente caracter sin avanzar
char Lexer::peekNext() const
{
    if (current + 1 >= source.length())
        return '\0';
    return source[current + 1];
}

// Verifica si el siguiente caracter es el esperado y avanza
bool Lexer::match(char expected)
{
    if (isAtEnd())
        return false;
    if (source[current] != expected)
        return false;
    current++;
    return true;
}

// Salta espacios en blanco y comentarios
void Lexer::skipWhitespace()
{
    while (!isAtEnd())
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            line++;
            advance();
            break;
        default:
            return;
        }
    }
}

// Crea un token con el tipo y lexema
Token Lexer::makeToken(TokenType type)
{
    // Nota- verifcar luego la logica de columna para manejo d errores
    std::string text = source.substr(start, current - start);
    return Token(type, text, line, start);
}

// Crea un token de error con el mensaje
Token Lexer::errorToken(const std::string &message)
{
    return Token(TokenType::EOF_TOKEN, message, line, current);
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    while (!isAtEnd())
    {
        start = current;
        scanToken(tokens);
    }
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", line, current));
    return tokens;
}

void Lexer::scanToken(std::vector<Token> &tokens)
{
    skipWhitespace();

    if (isAtEnd())
        return;

    char c = advance();
    if (std::isdigit(c))
    {
        numberToken(tokens);
    }
    else if (std::isalpha(c) || c == '_')
    {
        identifierToken(tokens);
    }
    else
    {
        // Reconocer operadores simples y simbolos
        switch (c)
        {
        case '+':
            tokens.push_back(makeToken(TokenType::Plus));
            break;
        case '-':
            tokens.push_back(makeToken(TokenType::Minus));
            break;
        case '*':
            tokens.push_back(makeToken(TokenType::Multiply));
            break;
        case '/':
            tokens.push_back(makeToken(TokenType::Divide));
            break;
        case '=':
            tokens.push_back(match('=') ? makeToken(TokenType::Equal_Equal)
                                        : makeToken(TokenType::Equal));
            break;
        case '!':
            tokens.push_back(match('=') ? makeToken(TokenType::Not_Equal)
                                        : errorToken("Caracter inesperado '!'"));
            break;
        case '<':
            tokens.push_back(match('=') ? makeToken(TokenType::Less_Equal)
                                        : makeToken(TokenType::Less));
            break;
        case '>':
            tokens.push_back(match('=') ? makeToken(TokenType::Greater_Equal)
                                        : makeToken(TokenType::Greater));
            break;
        case '(':
            tokens.push_back(makeToken(TokenType::Left_Paren));
            break;
        case ')':
            tokens.push_back(makeToken(TokenType::Right_Paren));
            break;
        case '{':
            tokens.push_back(makeToken(TokenType::Left_Brace));
            break;
        case '}':
            tokens.push_back(makeToken(TokenType::Right_Brace));
            break;
        case ';':
            tokens.push_back(makeToken(TokenType::Semicolon));
            break;
        case ',':
            tokens.push_back(makeToken(TokenType::Comma));
            break;
        case '"':
            stringToken(tokens);
            break;
        default:
            tokens.push_back(errorToken("Caracter inesperado."));
            break;
        }
    }
}

// Es un number token
void Lexer::numberToken(std::vector<Token> &tokens)
{
    while (std::isdigit(peek()))
        advance();

    // decimal
    if (peek() == '.' && std::isdigit(peekNext()))
    {
        advance(); // consume el punto
        while (std::isdigit(peek()))
            advance();
    }

    tokens.push_back(makeToken(TokenType::Number));
}

// Identificadores y keywords
void Lexer::identifierToken(std::vector<Token> &tokens)
{
    while (std::isalnum(peek()) || peek() == '_')
        advance();

    // agregar verificación de palabras clave en función del lexema ??????
    tokens.push_back(makeToken(TokenType::Identifier));
}

// Es un String token
void Lexer::stringToken(std::vector<Token> &tokens)
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n')
            line++;
        advance();
    }

    if (isAtEnd())
    {
        tokens.push_back(errorToken("Cadena no terminada."));
        return;
    }

    advance(); // consume la comilla final

    tokens.push_back(makeToken(TokenType::String));
}
