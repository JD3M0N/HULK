#pragma once
#include <string>
#include <vector>
#include <regex> // Added for std::regex
#include "token.h"

class Regex;

class Lexer
{
public:
    // Constructor que recibe un string
    Lexer(const std::string &source);

    // Metodo que returna la lista de tokens
    std::vector<Token> tokenize();

private:
    std::string source; // Codigo a analizar
    int start = 0;      // Start index
    int current = 0;    // Current index
    int line = 1;       // Seguimiento de la linea (para manejo de errores multilinea)

    // Auxiliares:
    bool isAtEnd() const;      // Verifica si se ha llegado al final del string
    char advance();            // Avanza un caracter
    char peek() const;         // Mira el actual caracter sin avanzar
    char peekNext() const;     // Mira el siguiente caracter sin avanzar
    bool match(char expected); // Verifica si el siguiente caracter es el esperado y avanza
    void skipWhitespace();     // Salta espacios en blanco y comentarios

    // std::string matchRegexToken(const std::regex &regex);
    std::string matchRegexToken(const Regex &regex);

    // Metodos para reconocer tokens
    Token makeToken(TokenType type);              // Crea un token con el tipo y lexema
    Token errorToken(const std::string &message); // Crea un token de error con el mensaje

    void scanToken(std::vector<Token> &tokens);
    void stringToken(std::vector<Token> &tokens);
    void numberToken(std::vector<Token> &tokens);
    void identifierToken(std::vector<Token> &tokens);
};
