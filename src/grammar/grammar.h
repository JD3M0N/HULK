// grammar.h
#ifndef HULK_GRAMMAR_H
#define HULK_GRAMMAR_H

#include <string>
#include <vector>
#include <unordered_map>

/// Una producción: lhs → rhs[0] rhs[1] … rhs[n-1]
struct Production
{
    std::string lhs;
    std::vector<std::string> rhs;
};

/// Clase que almacena la gramática de HULK
class Grammar
{
public:
    Grammar();

    /// Devuelve todas las producciones cuyo lado izquierdo es `nonTerminal`
    const std::vector<Production> &
    getProductions(const std::string &nonTerminal) const;

    /// Símbolo inicial de la gramática
    const std::string &getStartSymbol() const;

    /// Comprueba si un símbolo es no terminal
    bool isNonTerminal(const std::string &symbol) const;

    /// Comprueba si un símbolo es terminal
    bool isTerminal(const std::string &symbol) const;

    /// Lista de no terminales
    const std::vector<std::string> &getNonTerminals() const;

    /// Lista de terminales
    const std::vector<std::string> &getTerminals() const;

private:
    void initGrammar();

    std::string startSymbol;
    std::vector<std::string> nonTerminals;
    std::vector<std::string> terminals;
    std::unordered_map<std::string, std::vector<Production>> productions;
};

#endif // HULK_GRAMMAR_H
