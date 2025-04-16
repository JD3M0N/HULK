#ifndef REGEX_H
#define REGEX_H

#include <string>
#include <vector>
#include <unordered_set>

// Estructura que representa un "token" de la expresion regular
struct RegexToken {
    char c;       // Caracter literal o '.' para comodin (cualquier caracter)
    bool star;    // Indica si está seguido por '*' (0 o mas repeticiones)
};

class Regex {
    public:
        // Constructor: compila el patrón (por ejemplo, "a*b.c")
        Regex(const std::string &pattern);

        // Devuelve true si el texto (entrada) coincide por completo con el patrón
        bool matches(const std::string &text) const;

    private:
        std::vector<RegexToken> tokens; // Secuencia resultante del parseo del patrón

        // Parsea el patrón y llena la secuencia de tokens
        void parsePattern(const std::string &pattern);

        // Función auxiliar: dado un conjunto de estados (índices en tokens),
        // devuelve su cierre-ε (para tokens que tienen '*' se permite saltarlos)
        std::unordered_set<int> epsilonClosure(const std::unordered_set<int>& states) const;
};

#endif // REGEX_H
