#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cmath>
#include "error_handler.hpp"

// Clase base para todos los nodos del AST
class Node {
protected:
    SourceLocation location;
    
public:
    Node(const SourceLocation& loc = SourceLocation()) : location(loc) {}
    virtual ~Node() = default;
    
    // Método que deben implementar los nodos para evaluarse
    virtual double evaluate() = 0;
    
    SourceLocation getLocation() const { return location; }
};

// Clase para representar números
class NumberNode : public Node {
private:
    double value;
    
public:
    NumberNode(double val, const SourceLocation& loc = SourceLocation()) 
        : Node(loc), value(val) {}
    
    double evaluate() override {
        return value;
    }
};

// Clase para operaciones binarias (+, -, *, /, etc)
class BinaryOpNode : public Node {
private:
    char op;
    Node* left;
    Node* right;
    
public:
    BinaryOpNode(char operation, Node* l, Node* r, const SourceLocation& loc = SourceLocation()) 
        : Node(loc), op(operation), left(l), right(r) {}
    
    ~BinaryOpNode() {
        delete left;
        delete right;
    }
    
    double evaluate() override {
        double lval = left->evaluate();
        double rval = right->evaluate();
        
        switch(op) {
            case '+': return lval + rval;
            case '-': return lval - rval;
            case '*': return lval * rval;
            case '/': 
                if (rval == 0) {
                    reportRuntimeError("División por cero", location);
                }
                return lval / rval;
            case '^': return pow(lval, rval);
            case '%': return fmod(lval, rval);
            default:
                reportRuntimeError("Operador desconocido", location);
                return 0;
        }
    }
};