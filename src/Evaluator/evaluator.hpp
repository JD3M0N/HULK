// evaluator.hpp
#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#include <iostream>
#include <unordered_map>
#include <stdexcept>

#include "../AST/ast.hpp"
#include "../Value/enumerable.hpp"
#include "../Value/iterable.hpp"
#include "../Value/value.hpp"
#include "../Scope/scope.hpp"

struct EvaluatorVisitor : StmtVisitor, ExprVisitor
{
    Value lastValue{0.0};
    
    Scope<Value>::Ptr env;

    std::unordered_map<std::string, FunctionDecl *> functions;
    std::unordered_map<std::string, ClassDecl *> classes;
    Value currentSelf; // solo válido dentro de métodos
    ClassDecl *currentClass = nullptr;

    EvaluatorVisitor()
    {
        // Inicializar con un scope "global" sin padre
        env = std::make_shared<Scope<Value>>(nullptr);
    }

    // Función helper para actualizar variables
    void updateVariable(const std::string& name, const Value& newVal) {
        try {
            env->update(name, newVal);
        } catch (const std::runtime_error&) {
            throw std::runtime_error("Variable no encontrada para actualización: " + name);
        }
    }

    // Programa: recorre stmt a stmt
    void visit(Program *p) override
    {
        // Primero registrar TODAS las funciones
        for (auto &s : p->stmts)
        {
            auto *f = dynamic_cast<FunctionDecl *>(s.get());
            if (f)
                functions[f->name] = f;
        }

        // Luego registrar TODAS las clases
        for (auto &s : p->stmts)
        {
            auto *c = dynamic_cast<ClassDecl *>(s.get());
            if (c)
                classes[c->name] = c;
        }

        // Luego ejecutar todo (incluyendo funciones si hay recursión)
        for (auto &s : p->stmts)
        {
            s->accept(this);
        }
    }

    // StmtVisitor:
    void visit(ExprStmt *e) override
    {
        e->expr->accept(this);
    }

    // ExprVisitor:
    void visit(NumberExpr *e) override
    {
        lastValue = Value(e->value);
    }

    void visit(StringExpr *e) override
    {
        lastValue = Value(e->value);
    }

    void visit(BooleanExpr *expr) override
    {
        lastValue = Value(expr->value);
    }

    void visit(UnaryExpr *e) override
    {
        e->operand->accept(this);
        lastValue = Value(-lastValue.asNumber());
    }

    void visit(BinaryExpr *e) override
    {
        e->left->accept(this);
        Value l = lastValue;
        e->right->accept(this);
        Value r = lastValue;
        
        switch (e->op)
        {
        case BinaryExpr::OP_ADD:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de suma deben ser números");
            lastValue = Value(l.asNumber() + r.asNumber());
            break;
        case BinaryExpr::OP_SUB:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de resta deben ser números");
            lastValue = Value(l.asNumber() - r.asNumber());
            break;
        case BinaryExpr::OP_MUL:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de multiplicación deben ser números");
            lastValue = Value(l.asNumber() * r.asNumber());
            break;
        case BinaryExpr::OP_DIV:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de división deben ser números");
            if (r.asNumber() == 0)
                throw std::runtime_error("División por cero");
            lastValue = Value(l.asNumber() / r.asNumber());
            break;
        case BinaryExpr::OP_MOD:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de módulo deben ser números");
            if (r.asNumber() == 0)
                throw std::runtime_error("Módulo por cero");
            lastValue = Value(fmod(l.asNumber(), r.asNumber()));
            break;
        case BinaryExpr::OP_POW:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de potencia deben ser números");
            lastValue = Value(pow(l.asNumber(), r.asNumber()));
            break;
        case BinaryExpr::OP_LT:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de comparación deben ser números");
            lastValue = Value(l.asNumber() < r.asNumber());
            break;
        case BinaryExpr::OP_GT:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de comparación deben ser números");
            lastValue = Value(l.asNumber() > r.asNumber());
            break;
        case BinaryExpr::OP_LE:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de comparación deben ser números");
            lastValue = Value(l.asNumber() <= r.asNumber());
            break;
        case BinaryExpr::OP_GE:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de comparación deben ser números");
            lastValue = Value(l.asNumber() >= r.asNumber());
            break;
        case BinaryExpr::OP_EQ:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de igualdad deben ser números");
            lastValue = Value(l.asNumber() == r.asNumber());
            break;
        case BinaryExpr::OP_NEQ:
            if (!l.isNumber() || !r.isNumber())
                throw std::runtime_error("Operandos de desigualdad deben ser números");
            lastValue = Value(l.asNumber() != r.asNumber());
            break;
        case BinaryExpr::OP_OR:
            if (!l.isBool() || !r.isBool())
                throw std::runtime_error("Operandos de OR deben ser booleanos");
            lastValue = Value(l.asBool() || r.asBool());
            break;
        case BinaryExpr::OP_AND:
            if (!l.isBool() || !r.isBool())
                throw std::runtime_error("Operandos de AND deben ser booleanos");
            lastValue = Value(l.asBool() && r.asBool());
            break;
        case BinaryExpr::OP_CONCAT:
            if (!l.isString() || !r.isString())
                throw std::runtime_error("Operandos de concatenación deben ser strings");
            lastValue = Value(l.asString() + r.asString());
            break;
        default:
            throw std::runtime_error("Operador binario no soportado");
        }
    }

    void visit(CallExpr *e) override
    {
        // Evaluar argumentos
        std::vector<Value> argVals;
        for (auto &arg : e->args)
        {
            arg->accept(this);
            argVals.push_back(lastValue);
        }

        // Funciones nativas
        if (e->callee == "print")
        {
            if (argVals.size() != 1)
                throw std::runtime_error("print() requiere exactamente 1 argumento");
            
            Value arg = argVals[0];
            if (arg.isNumber())
                std::cout << arg.asNumber();
            else if (arg.isString())
                std::cout << arg.asString();
            else if (arg.isBool())
                std::cout << (arg.asBool() ? "true" : "false");
            else
                throw std::runtime_error("Tipo no imprimible");
            
            lastValue = Value(0.0); // print retorna 0
            return;
        }

        if (e->callee == "sqrt")
        {
            if (argVals.size() != 1)
                throw std::runtime_error("sqrt() requiere exactamente 1 argumento");
            if (!argVals[0].isNumber())
                throw std::runtime_error("sqrt() requiere un número");
            if (argVals[0].asNumber() < 0)
                throw std::runtime_error("sqrt() de número negativo");
            lastValue = Value(sqrt(argVals[0].asNumber()));
            return;
        }

        // Más funciones nativas...
        if (e->callee == "sin")
        {
            if (argVals.size() != 1 || !argVals[0].isNumber())
                throw std::runtime_error("sin() requiere un número");
            lastValue = Value(sin(argVals[0].asNumber()));
            return;
        }

        if (e->callee == "cos")
        {
            if (argVals.size() != 1 || !argVals[0].isNumber())
                throw std::runtime_error("cos() requiere un número");
            lastValue = Value(cos(argVals[0].asNumber()));
            return;
        }

        if (e->callee == "rand")
        {
            if (argVals.size() != 0)
                throw std::runtime_error("rand() no requiere argumentos");
            lastValue = Value(static_cast<double>(rand()) / RAND_MAX);
            return;
        }

        if (e->callee == "PI")
        {
            if (argVals.size() != 0)
                throw std::runtime_error("PI() no requiere argumentos");
            lastValue = Value(M_PI);
            return;
        }

        if (e->callee == "E")
        {
            if (argVals.size() != 0)
                throw std::runtime_error("E() no requiere argumentos");
            lastValue = Value(M_E);
            return;
        }

        // Funciones definidas por el usuario
        auto it = functions.find(e->callee);
        if (it == functions.end())
            throw std::runtime_error("Función no definida: " + e->callee);

        FunctionDecl *func = it->second;
        if (argVals.size() != func->params.size())
            throw std::runtime_error("Número incorrecto de argumentos para " + e->callee);

        // Crear nuevo scope para la función
        auto savedEnv = env;
        env = std::make_shared<Scope<Value>>(savedEnv);

        // Asignar parámetros
        for (size_t i = 0; i < func->params.size(); ++i)
        {
            env->declare(func->params[i], argVals[i]);
        }

        // Ejecutar cuerpo
        func->body->accept(this);
        Value result = lastValue;

        // Restaurar entorno
        env = savedEnv;
        lastValue = result;
    }

    void visit(VariableExpr *expr) override
    {
        try {
            lastValue = env->lookup(expr->name);
        } catch (const std::runtime_error&) {
            throw std::runtime_error("Variable no definida: " + expr->name);
        }
    }

    void visit(LetExpr *expr) override
    {
        // Evaluar inicializador
        expr->initializer->accept(this);
        Value initVal = lastValue;

        // Crear nuevo scope
        auto parent = env;
        env = std::make_shared<Scope<Value>>(parent);
        env->declare(expr->name, initVal);

        // Ejecutar cuerpo
        expr->body->accept(this);
        
        // Restaurar scope
        env = parent;
    }

    void visit(AssignExpr *expr) override
    {
        // Evaluar valor
        expr->value->accept(this);
        Value newVal = lastValue;

        // Actualizar variable
        try {
            updateVariable(expr->name, newVal);
        } catch (const std::runtime_error&) {
            throw std::runtime_error("Variable no encontrada para asignación: " + expr->name);
        }

        lastValue = newVal;
    }

    void visit(FunctionDecl *f) override
    {
        // Las funciones ya están registradas, no hacer nada aquí
    }

    void visit(IfExpr *e) override
    {
        e->condition->accept(this);
        if (!lastValue.isBool())
            throw std::runtime_error("Condición de if debe ser booleana");

        if (lastValue.asBool())
            e->thenBranch->accept(this);
        else
            e->elseBranch->accept(this);
    }

    void visit(ExprBlock *b) override
    {
        auto parent = env;
        env = std::make_shared<Scope<Value>>(parent);
        
        for (auto &stmt : b->stmts)
        {
            stmt->accept(this);
        }
        
        env = parent;
    }

    void visit(WhileExpr *expr) override
    {
        auto parent = env;
        env = std::make_shared<Scope<Value>>(parent);
        
        while (true)
        {
            expr->condition->accept(this);
            if (!lastValue.isBool())
                throw std::runtime_error("Condición de while debe ser booleana");
            
            if (!lastValue.asBool())
                break;
            
            expr->body->accept(this);
        }
        
        env = parent;
    }

    void visit(ClassDecl *c) override
    {
        // Las clases ya están registradas, no hacer nada aquí
    }

    void visit(NewExpr *expr) override
    {
        auto it = classes.find(expr->typeName);
        if (it == classes.end())
            throw std::runtime_error("Clase no definida: " + expr->typeName);

        ClassDecl *cls = it->second;
        auto obj = std::make_shared<ObjectValue>(expr->typeName, cls);

        // Evaluar argumentos del constructor
        std::vector<Value> argVals;
        for (auto &arg : expr->args)
        {
            arg->accept(this);
            argVals.push_back(lastValue);
        }

        // Inicializar atributos con sus valores por defecto
        for (auto &attr : cls->attributes)
        {
            attr.second->accept(this);
            obj->fields->declare(attr.first, lastValue);
        }

        lastValue = Value(obj);
    }

    void visit(SelfExpr *) override
    {
        lastValue = currentSelf;
    }

    void visit(BaseExpr *) override
    {
        // Implementación básica
        lastValue = currentSelf;
    }

    void visit(MemberAccessExpr *expr) override
    {
        expr->object->accept(this);
        if (!lastValue.isObject())
            throw std::runtime_error("Acceso a miembro en non-objeto");

        auto obj = lastValue.asObject();
        try {
            lastValue = obj->fields->lookup(expr->member);
        } catch (const std::runtime_error&) {
            throw std::runtime_error("Miembro no encontrado: " + expr->member);
        }
    }

    void visit(MemberAssignExpr *expr) override
    {
        expr->object->accept(this);
        if (!lastValue.isObject())
            throw std::runtime_error("Asignación a miembro en non-objeto");

        auto obj = lastValue.asObject();
        expr->value->accept(this);
        Value v = lastValue;

        try {
            obj->fields->update(expr->member, v);
        } catch (const std::runtime_error&) {
            throw std::runtime_error("Miembro no encontrado para asignación: " + expr->member);
        }

        lastValue = v;
    }
};

#endif