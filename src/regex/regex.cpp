#include "regex.h"
#include <iostream>

// Parsea el patrón y construye el vector de tokens.
// En este motor sencillo, el patrón soporta:
//   - Caracteres literales (por ejemplo, 'a')
//   - El carácter '.' (comodín que coincide con cualquier carácter)
//   - El modificador '*' que se aplica al carácter inmediatamente anterior.
void Regex::parsePattern(const std::string &pattern)
{
    tokens.clear();
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        RegexToken token;
        token.c = pattern[i];
        token.star = false;
        if (i + 1 < pattern.size() && pattern[i + 1] == '*')
        {
            token.star = true;
            ++i; // Saltar el '*' ya procesado
        }
        tokens.push_back(token);
    }
}

Regex::Regex(const std::string &pattern)
{
    parsePattern(pattern);
}

// Calcula el "cierre epsilon" del conjunto de estados (índices en tokens).
// Si el token en la posición 'i' tiene '*' se puede saltar sin consumir un carácter,
// por lo tanto, se agrega el estado siguiente (i+1) al cierre.
std::unordered_set<int> Regex::epsilonClosure(const std::unordered_set<int> &states) const
{
    std::unordered_set<int> closure = states;
    bool added = true;
    while (added)
    {
        added = false;
        std::unordered_set<int> newStates;
        for (int state : closure)
        {
            if (state < tokens.size() && tokens[state].star)
            {
                int nextState = state + 1;
                if (closure.find(nextState) == closure.end())
                {
                    newStates.insert(nextState);
                }
            }
        }
        if (!newStates.empty())
        {
            closure.insert(newStates.begin(), newStates.end());
            added = true;
        }
    }
    return closure;
}

// Función de coincidencia:
// Se simula la ejecución de un autómata NFA sencillo:
//  - Los "estados" son índices en el vector tokens (con el estado final siendo tokens.size()).
//  - Se parte del estado 0 y se aplica el cierre ε (para saltar tokens con '*' que pueden ser omitidos).
//  - Por cada carácter del texto se actualiza el conjunto de estados alcanzables.
bool Regex::matches(const std::string &text) const
{
    size_t i = 0, j = 0; // i: índice en el texto; j: índice en tokens
    int starIdx = -1;    // Índice del último token con '*' (si se encontró)
    size_t match = 0;    // Posición en el texto cuando se encontró la última estrella

    // Mientras queden caracteres en el texto
    while (i < text.size())
    {
        // Si existe un token en j y este no es '*', y el token concuerda con el carácter actual
        if (j < tokens.size() && !tokens[j].star &&
            (tokens[j].c == text[i] || tokens[j].c == '.'))
        {
            i++;
            j++;
        }
        // Si el token actual es un '*' (es decir, su flag star está activo)
        else if (j < tokens.size() && tokens[j].star)
        {
            // Guardamos el índice de la estrella y la posición actual del texto
            starIdx = j;
            match = i;
            j++; // Avanzamos en el patrón, tratando la estrella como que consume 0 caracteres
        }
        // Si hubo una estrella previamente, retrocedemos un poco (backtracking lineal)
        else if (starIdx != -1)
        {
            // Volvemos al token justo después de la última estrella encontrada
            j = starIdx + 1;
            // Incrementamos la cantidad de caracteres que consume la parte de la estrella
            match++;
            i = match;
        }
        else
        {
            // No hay match posible
            return false;
        }
    }
    // Terminado el texto, deben quedar únicamente tokens que puedan consumir 0 caracteres (tokens con '*')
    while (j < tokens.size() && tokens[j].star)
    {
        j++;
    }
    return j == tokens.size();
}