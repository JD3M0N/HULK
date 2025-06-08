#pragma once
#include <string>
#include <stdexcept>

// Enumeración de tipos de tokens
enum class TokenType {
    NUMBER,
    IDENTIFIER,
    STRING,
    
    // Operadores
    PLUS,        // +
    MINUS,       // -
    MULTIPLY,    // *
    DIVIDE,      // /
    POWER,       // ^
    MOD,         // %
    CONCAT,      // @
    
    // Comparadores
    EQUAL,       // ==
    NOT_EQUAL,   // !=
    LESS,        // <
    GREATER,     // >
    LESS_EQUAL,  // <=
    GREATER_EQUAL, // >=
    
    // Lógicos
    AND,         // &
    OR,          // |
    
    // Asignación
    ASSIGN,      // = o :=
    
    // Delimitadores
    LPAREN,      // (
    RPAREN,      // )
    LBRACE,      // {
    RBRACE,      // }
    COMMA,       // ,
    SEMICOLON,   // ;
    
    // Palabras clave
    LET,
    IN,
    PRINT,
    FUNCTION,
    IF,
    ELSE,
    ELIF,
    WHILE,
    FOR,
    ARROW,       // =>
    
    // Fin de archivo
    END_OF_FILE
};

// Estructura para almacenar la ubicación de un token en el código fuente
struct TokenLocation {
    int line;
    int column;
    
    TokenLocation(int l = 1, int c = 1) : line(l), column(c) {}
};

// Clase de Token sin usar variant
class Token {
private:
    TokenType type;
    double numberValue;
    std::string stringValue;
    bool hasNumberValue;
    bool hasStringValue;
    TokenLocation location;
    
public:
    // Constructor para tokens sin valor
    Token(TokenType t, int line = 1, int column = 1)
        : type(t), 
          numberValue(0.0), 
          stringValue(""), 
          hasNumberValue(false), 
          hasStringValue(false), 
          location(line, column) {}
    
    // Constructor para tokens con valor numérico
    Token(TokenType t, double val, int line = 1, int column = 1)
        : type(t), 
          numberValue(val), 
          stringValue(""), 
          hasNumberValue(true), 
          hasStringValue(false), 
          location(line, column) {}
    
    // Constructor para tokens con valor de cadena
    Token(TokenType t, const std::string& val, int line = 1, int column = 1)
        : type(t), 
          numberValue(0.0), 
          stringValue(val), 
          hasNumberValue(false), 
          hasStringValue(true), 
          location(line, column) {}
    
    // Métodos getter
    TokenType getType() const { return type; }
    TokenLocation getLocation() const { return location; }
    
    bool hasValue() const { 
        return hasNumberValue || hasStringValue; 
    }
    
    double getNumberValue() const {
        if (!hasNumberValue) {
            throw std::runtime_error("Token no contiene un valor numérico");
        }
        return numberValue;
    }
    
    std::string getStringValue() const {
        if (!hasStringValue) {
            throw std::runtime_error("Token no contiene un valor de cadena");
        }
        return stringValue;
    }
    
    bool isNumber() const { return hasNumberValue; }
    bool isString() const { return hasStringValue; }
};