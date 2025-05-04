#include "AST.h"
#include <cmath>
#include <iostream>

// Definición de la tabla estática de funciones
const std::map<std::string, std::function<double(double)>> FunctionCallNode::funcs = {
    {"sin", [](double x)
     { return std::sin(x); }},
    {"cos", [](double x)
     { return std::cos(x); }},
    {"tan", [](double x)
     { return std::tan(x); }},
    {"sqrt", [](double x)
     {
         if (x < 0)
             throw std::runtime_error("sqrt de negativo");
         return std::sqrt(x);
     }}};

// Constructor y destructor de BinaryOpNode
BinaryOpNode::BinaryOpNode(char o, ASTPtr l, ASTPtr r)
    : op(o), left(l), right(r) {}
BinaryOpNode::~BinaryOpNode()
{
    delete left;
    delete right;
}

// Constructor y destructor de FunctionCallNode
FunctionCallNode::FunctionCallNode(std::string n, ASTPtr a)
    : name(std::move(n)), arg(a) {}
FunctionCallNode::~FunctionCallNode()
{
    delete arg;
}

// Implementaciones de evaluate() con debug
double NumberNode::evaluate() const
{
    std::cerr << "[AST] NumberNode -> " << value << "\n";
    return value;
}

double BinaryOpNode::evaluate() const
{
    std::cerr << "[AST] Enter BinaryOpNode op='" << op << "'\n";
    double L = left->evaluate();
    double R = right->evaluate();
    double res;
    switch (op)
    {
    case '+':
        res = L + R;
        break;
    case '-':
        res = L - R;
        break;
    case '*':
        res = L * R;
        break;
    case '/':
        if (R == 0)
            throw std::runtime_error("división por cero");
        res = L / R;
        break;
    default:
        throw std::runtime_error("operador desconocido");
    }
    std::cerr << "[AST] Exit BinaryOpNode -> " << res << "\n";
    return res;
}

double FunctionCallNode::evaluate() const
{
    std::cerr << "[AST] Enter FunctionCallNode '" << name << "'\n";
    auto it = funcs.find(name);
    if (it == funcs.end())
        throw std::runtime_error("función desconocida: " + name);
    double argVal = arg->evaluate();
    double res = it->second(argVal);
    std::cerr << "[AST] Exit FunctionCallNode -> " << res << "\n";
    return res;
}
