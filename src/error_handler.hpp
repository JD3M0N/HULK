#pragma once
#include <string>
#include <stdexcept>
#include <iostream>

// Estructura para guardar información de ubicación en el código
class SourceLocation {
public:
    int line;
    int column;
    
    SourceLocation(int l = 0, int c = 0) : line(l), column(c) {}
    
    std::string toString() const {
        return "línea " + std::to_string(line) + ", columna " + std::to_string(column);
    }
};

// Excepciones específicas para diferentes tipos de errores
class LexicalError : public std::runtime_error {
public:
    LexicalError(const std::string& msg) : std::runtime_error(msg) {}
};

class SyntaxError : public std::runtime_error {
public:
    SyntaxError(const std::string& msg) : std::runtime_error(msg) {}
};

class SemanticError : public std::runtime_error {
public:
    SemanticError(const std::string& msg) : std::runtime_error(msg) {}
};

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

// Funciones auxiliares para lanzar errores
inline void reportLexicalError(const std::string& msg, const SourceLocation& loc) {
    throw LexicalError("Error léxico en " + loc.toString() + ": " + msg);
}

inline void reportSyntaxError(const std::string& msg, const SourceLocation& loc) {
    throw SyntaxError("Error sintáctico en " + loc.toString() + ": " + msg);
}

inline void reportSemanticError(const std::string& msg, const SourceLocation& loc) {
    throw SemanticError("Error semántico en " + loc.toString() + ": " + msg);
}

inline void reportRuntimeError(const std::string& msg, const SourceLocation& loc) {
    throw RuntimeError("Error de ejecución en " + loc.toString() + ": " + msg);
}