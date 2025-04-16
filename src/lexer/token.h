#pragma once
#include <string>
/* 
Literales numéricos (Number) : 42, 3.14

Literales de cadena (string) : "hola mundo"

Literales booleanos (Boolean) : True, False

Identificadores : nombres de variables y funciones

Operadores aritméticos :  +, -, *, / 

Operadores booleanos y de comparación : ==, !=, <, >, <=, >=, &&, ||

Operador de asignación : =

Separadores y símbolos : paréntesis, llaves, punto y coma, etc.
*/

enum class TokenType {
    // Literales
    Number,
    String,
    Boolean,
    
    // Identificadores y palabras clave ????
    Identifier,
    Keyword,  // distinguir entre ambos ???

    // Operadores
    Plus,       // +
    Minus,      // -
    Multiply,       // *
    Divide,      // /
    Equal,      // =
    Equal_Equal, // ==
    Not_Equal,  // !=
    Less,        // <
    Less_Equal,  // <=
    Greater,     // >
    Greater_Equal, // >=
    And,         // &&
    Or,          // ||
    
    // Separadores y otros símbolos
    Left_Paren,   // (
    Right_Paren,  // )
    Left_Brace,   // {
    Right_Brace,  // }
    Semicolon,    // ;
    Comma,        // ,
    
    // Fin de archivo
    EOF_TOKEN
};

// Estructura q almacena el token junto a sus atributos
struct Token {
    TokenType type;
    std::string lexeme;  // El string original a lexear
    int line;            // Linea para manejo de errores multilinea
    int column;          // Pos donde aparece el token para manejo de errores

    Token(TokenType type, std::string lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
};
