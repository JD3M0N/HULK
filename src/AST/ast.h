#ifndef AST_H
#define AST_H

#include <string>
#include <map>
#include <functional>
#include <stdexcept>

// Forward declaration y typedef
class ASTNode;
using ASTPtr = ASTNode *;

// Nodo base
class ASTNode
{
public:
    virtual ~ASTNode() = default;
    virtual double evaluate() const = 0;
};

// Nodo de número
class NumberNode : public ASTNode
{
    double value;

public:
    explicit NumberNode(double v) : value(v) {}
    double evaluate() const override; // solo declaración
};

// Nodo binario
class BinaryOpNode : public ASTNode
{
    char op;
    ASTPtr left, right;

public:
    BinaryOpNode(char o, ASTPtr l, ASTPtr r);
    ~BinaryOpNode();
    double evaluate() const override; // solo declaración
};

// Nodo de llamada a función
class FunctionCallNode : public ASTNode
{
    std::string name;
    ASTPtr arg;
    // Declaración de la tabla de funciones (definida en AST.cpp)
    static const std::map<std::string, std::function<double(double)>> funcs;

public:
    FunctionCallNode(std::string n, ASTPtr a);
    ~FunctionCallNode();
    double evaluate() const override; // solo declaración
};

#endif // AST_H
