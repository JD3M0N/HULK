// evaluator.hpp
#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#define _USE_MATH_DEFINES // This ensures M_PI and M_E are defined
#include <cmath>

// Fallback definitions if M_PI and M_E are still not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

#include <iostream>
#include <unordered_map>

#include "../AST/ast.hpp"
#include "../Value/enumerable.hpp"
#include "../Value/iterable.hpp"
#include "../Value/value.hpp"
#include "env_frame.hpp"
#include "../Errors/error_handler.hpp"
#include "../Errors/error_types/type_error.hpp"
#include "../Errors/error_types/name_error.hpp"
#include "../Errors/error_types/runtime_error.hpp"
#include "../Errors/error_types/value_error.hpp"

static std::shared_ptr<EnvFrame> makeChild(const std::shared_ptr<EnvFrame>& parent)
{
    return std::make_shared<EnvFrame>(parent);
}

struct EvaluatorVisitor : StmtVisitor, ExprVisitor
{
    Value lastValue{0.0};
    // En vez de un mapa plano, un puntero a EnvFrame
    std::shared_ptr<EnvFrame> env;

    std::unordered_map<std::string, FunctionDecl *> functions;

    std::unordered_map<std::string, ClassDecl*> classes;
    Value currentSelf;              // solo válido dentro de métodos
    ClassDecl* currentClass = nullptr;

    EvaluatorVisitor()
    {
        // Inicializar con un frame “global” sin padre
        env = std::make_shared<EnvFrame>(nullptr);
    }

    // Programa: recorre stmt a stmt
    void
    visit(Program *p) override
    {
        for (auto &s : p->stmts)
        {
            if (auto *cd = dynamic_cast<ClassDecl*>(s.get()))
            {
                cd->accept(this);
            }
        }
        // Primero registrar TODAS las funciones
        for (auto &s : p->stmts)
        {
            if (auto *fd = dynamic_cast<FunctionDecl *>(s.get()))
            {
                fd->accept(this); // esto registra la función en el mapa
            }
        }

        // Luego ejecutar todo (incluyendo funciones si hay recursión)
        for (auto &s : p->stmts)
        {
            if (!dynamic_cast<FunctionDecl *>(s.get()))
            {
                s->accept(this);
            }
        }
    }

    // StmtVisitor:

    void
    visit(ExprStmt *e) override
    {
        e->expr->accept(this);
    }

    // ExprVisitor:
    void
    visit(NumberExpr *e) override
    {
        lastValue = Value(e->value);
    }

    void
    visit(StringExpr *e) override
    {
        lastValue = Value(e->value);
    }

    void
    visit(BooleanExpr *expr) override
    {
        lastValue = Value(expr->value);
    }

    void
    visit(UnaryExpr *e) override
    {
        e->operand->accept(this);
        lastValue = Value(-lastValue.asNumber());
    }

    void
    visit(BinaryExpr *e) override
    {
        e->left->accept(this);
        Value l = lastValue;
        e->right->accept(this);
        Value r = lastValue;
        switch (e->op)
        {
        case BinaryExpr::OP_ADD:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "ambos miembros en una suma deben ser números",
                    Location(e->getLine(), e->getColumn())
                );
                return; 
            }
            lastValue = Value(l.asNumber() + r.asNumber());
            break;
        case BinaryExpr::OP_SUB:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "ambos miembros en una resta deben ser números",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() - r.asNumber());
            break;
        // Operador de multiplicación
        case BinaryExpr::OP_MUL:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en multiplicación: se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() * r.asNumber());
            break;
        case BinaryExpr::OP_DIV:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en división: se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            if (r.asNumber() == 0)
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "División por cero no permitida",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() / r.asNumber());
            break;
        // Para operación módulo
        case BinaryExpr::OP_MOD:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Ambos operandos del operador '%' deben ser números",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            if (r.asNumber() == 0)
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "Operación de módulo por cero no permitida",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(fmod(l.asNumber(), r.asNumber()));
            break;
        // Operador de potencia
        case BinaryExpr::OP_POW:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en potencia: se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(pow(l.asNumber(), r.asNumber()));
            break;
        // Operador menor que
        case BinaryExpr::OP_LT:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en comparación '<': se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() < r.asNumber() ? true : false);
            break;
        // Operador mayor que
        case BinaryExpr::OP_GT:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en comparación '>': se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() > r.asNumber() ? true : false);
            break;
        // Operador menor o igual que
        case BinaryExpr::OP_LE:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en comparación '<=': se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() <= r.asNumber() ? true : false);
            break;
        // Operador mayor o igual que
        case BinaryExpr::OP_GE:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en comparación '>=': se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() >= r.asNumber() ? true : false);
            break;
        // Operador de igualdad
        case BinaryExpr::OP_EQ:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en comparación '==': se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() == r.asNumber() ? true : false);
            break;
        // Operador de desigualdad
        case BinaryExpr::OP_NEQ:
            if (!l.isNumber() || !r.isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en comparación '!=': se esperan dos números pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asNumber() != r.asNumber() ? true : false);
            break;
        // Operador OR lógico
        case BinaryExpr::OP_OR:
            if (!l.isBool() || !r.isBool())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en operador '||': se esperan dos booleanos pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asBool() || r.asBool() ? true : false);
            break;
        // Operador AND lógico
        case BinaryExpr::OP_AND:
            if (!l.isBool() || !r.isBool())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en operador '&&': se esperan dos booleanos pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asBool() && r.asBool() ? true : false);
            break;
        case BinaryExpr::OP_CONCAT:
            if (!l.isString() || !r.isString())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "Error de tipo en concatenación: se esperan dos cadenas pero se recibieron '" + 
                    l.getTypeName() + "' y '" + r.getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(l.asString() + r.asString());
            break;
        // Caso por defecto
        default:
            ErrorManager::getInstance().report<RuntimeError>(
                "Operador desconocido o no implementado",
                Location(e->getLine(), e->getColumn())
            );
            return;
        }
    }

    void
    visit(CallExpr *e) override
    {
        std::vector<Value> args;
        for (auto &arg : e->args)
        {
            arg->accept(this);
            args.push_back(lastValue);
        }

        // Funciones definidas por el usuario
        auto it = functions.find(e->callee);
        if (it != functions.end())
        {
            FunctionDecl *f = it->second;

            if (f->params.size() != args.size())
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "Número incorrecto de argumentos para función '" + f->name + "': " +
                    "esperados " + std::to_string(f->params.size()) + ", recibidos " + std::to_string(args.size()),
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }

            // Guardar entorno actual
            auto oldEnv = env;
            env = std::make_shared<EnvFrame>(oldEnv);

            // Asignar parámetros
            for (size_t i = 0; i < f->params.size(); ++i)
            {
                env->locals[f->params[i]] = args[i];
            }

            // Evaluar cuerpo
            f->body->accept(this);

            // Restaurar entorno
            env = std::move(oldEnv);

            return;
        }

        // Funciones nativas del lenguaje
        if (e->callee == "range")
        {
            if (args.size() != 2 || !args[0].isNumber() || !args[1].isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "La función 'range' espera 2 argumentos numéricos",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            double start = args[0].asNumber();
            double end = args[1].asNumber();
            auto rv = std::make_shared<RangeValue>(start, end);
            lastValue = Value(rv);
            return;
        }
        else if (e->callee == "iter")
        {
            if (args.size() != 1)
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "La función 'iter' espera exactamente 1 argumento",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            if (!args[0].isRange())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "La función 'iter' espera un valor iterable, pero recibió un '" + args[0].getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            auto rv = args[0].asRange();
            auto itr = rv->iter();
            lastValue = Value(itr);
            return;
        }
        else if (e->callee == "next")
        {
            if (args.size() != 1 || !args[0].isIterable())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "La función 'next'/'current' espera 1 argumento de tipo Iterable",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            auto itr = args[0].asIterable();
            bool hay = itr->next();
            lastValue = Value(hay);
            return;
        }
        else if (e->callee == "current")
        {
            if (args.size() != 1 || !args[0].isIterable())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "La función 'next'/'current' espera 1 argumento de tipo Iterable",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            auto itr = args[0].asIterable();
            lastValue = itr->current();
            return;
        }
        else if (e->callee == "print")
        {
            if (args.size() != 1)
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "La función 'print' espera exactamente 1 argumento",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            std::cout << args[0] << std::endl;
            lastValue = args[0];
            return;
        }
        else if (e->callee == "sqrt")
        {
            if (args.size() != 1)
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "La función 'sqrt' espera exactamente 1 argumento, pero se recibieron " + 
                    std::to_string(args.size()),
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            
            if (!args[0].isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "La función 'sqrt' espera un número, pero se recibió un '" + 
                    args[0].getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            
            if (args[0].asNumber() < 0)
            {
                ErrorManager::getInstance().report<ValueError>(
                    "No se puede calcular la raíz cuadrada de un número negativo: " + 
                    args[0].toString(),
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            
            lastValue = Value(std::sqrt(args[0].asNumber()));
            return;
        }
        else if (e->callee == "log")
        {
            if (args.size() == 1 || args.size() == 2)
            {
                // Caso de 1 argumento: log natural
                if (args.size() == 1)
                {
                    if (!args[0].isNumber())
                    {
                        ErrorManager::getInstance().report<TypeError>(
                            "La función 'log' espera un número, pero recibió un '" + args[0].getTypeName() + "'",
                            Location(e->getLine(), e->getColumn())
                        );
                        return;
                    }
                    
                    if (args[0].asNumber() <= 0)
                    {
                        ErrorManager::getInstance().report<ValueError>(
                            "No se puede calcular el logaritmo de un número no positivo: " + args[0].toString(),
                            Location(e->getLine(), e->getColumn())
                        );
                        return;
                    }
                    
                    lastValue = Value(std::log(args[0].asNumber()));
                    return;
                }
                
                // Caso de 2 argumentos: log en base específica
                double base = args[0].asNumber();
                double x = args[1].asNumber();
                
                if (base <= 0 || base == 1)
                {
                    ErrorManager::getInstance().report<ValueError>(
                        "Base inválida para logaritmo: debe ser positiva y distinta de 1",
                        Location(e->getLine(), e->getColumn())
                    );
                    return;
                }
                
                if (x <= 0)
                {
                    ErrorManager::getInstance().report<ValueError>(
                        "No se puede calcular el logaritmo de un número no positivo",
                        Location(e->getLine(), e->getColumn())
                    );
                    return;
                }
                
                lastValue = Value(std::log(x) / std::log(base));
                return;
            }
            else
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "La función 'log' espera 1 o 2 argumentos, pero recibió " + std::to_string(args.size()),
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
        }
        else if (e->callee == "sin")
        {
            if (args.size() != 1)
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "La función 'sin'/'cos' espera exactamente 1 argumento",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }

            if (!args[0].isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "La función 'sin'/'cos' espera un número, pero recibió un '" + args[0].getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(std::sin(args[0].asNumber()));
            return;
        }
        else if (e->callee == "cos")
        {
            if (args.size() != 1)
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "La función 'sin'/'cos' espera exactamente 1 argumento",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }

            if (!args[0].isNumber())
            {
                ErrorManager::getInstance().report<TypeError>(
                    "La función 'sin'/'cos' espera un número, pero recibió un '" + args[0].getTypeName() + "'",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(std::cos(args[0].asNumber()));
            return;
        }
        else if (e->callee == "rand")
        {
            lastValue = Value(static_cast<double>(rand()) / RAND_MAX);
            return;
        }
        else if (e->callee == "PI")
        {
            if (!args.empty())
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "La constante 'PI'/'E' no acepta argumentos",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(M_PI);
            return;
        }
        else if (e->callee == "E")
        {
            if (!args.empty())
            {
                ErrorManager::getInstance().report<RuntimeError>(
                    "La constante 'PI'/'E' no acepta argumentos",
                    Location(e->getLine(), e->getColumn())
                );
                return;
            }
            lastValue = Value(M_E);
            return;
        }
        else
        {
            ErrorManager::getInstance().report<NameError>(
                "Función no definida: '" + e->callee + "'",
                Location(e->getLine(), e->getColumn())
            );
            return;
        }
    }

    // for variable declarations
    void
    visit(VariableExpr *expr) override
    {
        if (!env->existsInChain(expr->name))
        {
            ErrorManager::getInstance().report<NameError>(
                "Variable no definida: '" + expr->name + "'",
                Location(expr->getLine(), expr->getColumn())
            );
            return;
        }
        
        // get() buscará en este frame y en los padres
        lastValue = env->get(expr->name);
    }

    // let in expressions
    void
    visit(LetExpr *expr) override
    {
        // 1) Evaluar la expresión del inicializador
        expr->initializer->accept(this);
        Value initVal = lastValue;

        // 2) Abrir un nuevo frame (scope hijo)
        auto oldEnv = env; // guardar el frame padre
        env = std::make_shared<EnvFrame>(oldEnv);

        // 3) Insertar la variable en el mapa local
        env->locals[expr->name] = initVal;

        // 4) Evaluar el cuerpo (es un Stmt)
        expr->body->accept(static_cast<StmtVisitor *>(this));
        Value result = lastValue;

        // 5) Al salir, restaurar el frame anterior
        env = std::move(oldEnv);

        // 6) El valor resultante de la expresión let es el valor devuelto
        lastValue = result;
    }

    // destructive assignment
    void
    visit(AssignExpr *expr) override
    {
        // Antes de asignar, evaluamos la expresión de la derecha:
        expr->value->accept(this);
        Value newVal = lastValue;

        // Verificar que exista en alguna parte (no crear nuevas automáticamente):
        if (!env->existsInChain(expr->name))
        {
            ErrorManager::getInstance().report<NameError>(
                "No se puede asignar a variable no declarada: '" + expr->name + "'",
                Location(expr->getLine(), expr->getColumn())
            );
            return;
        }
        // Llamamos a set() para que reasigne en el frame correspondiente:
        env->set(expr->name, newVal);
        lastValue = newVal;
    }

    // functions declaration
    void
    visit(FunctionDecl *f) override
    {
        if (functions.count(f->name))
            throw std::runtime_error("Funcion ya definida: " + f->name);
        functions[f->name] = f;
    }

    // if-else
    void
    visit(IfExpr *e) override
    {
        e->condition->accept(this);
        if (!lastValue.isBool())
        {
            ErrorManager::getInstance().report<TypeError>(
                "La condición de un if debe ser booleana",
                Location(e->condition->getLine(), e->condition->getColumn())
            );
            return;
        }

        if (lastValue.asBool())
        {
            e->thenBranch->accept(this);
        }
        else
        {
            e->elseBranch->accept(this);
        }
    }

    void
    visit(ExprBlock *b) override
    {
        // 1) Abrir un nuevo frame (scope hijo) antes de entrar al bloque
        auto oldEnv = env;
        env = std::make_shared<EnvFrame>(oldEnv);

        // 2) Evaluar cada sentencia dentro del bloque con este nuevo frame
        for (auto &stmt : b->stmts)
        {
            stmt->accept(this);
        }

        // 3) Restaurar el frame anterior al salir del bloque
        env = std::move(oldEnv);

        // lastValue queda con el valor del último statement ejecutado
    }

    void
    visit(WhileExpr *expr) override
    {
        Value result;
        while (true)
        {
            expr->condition->accept(this);
            if (!lastValue.isBool())
                throw std::runtime_error("La condición de un while debe ser booleana");
            if (!lastValue.asBool())
                break;

            expr->body->accept(this);
            result = lastValue;
        }
        lastValue = result;
    }

    void 
    visit(ClassDecl *c) override
    {
        classes[c->name] = c;
        // NOTA: no ejecutamos nada más: atributos se evaluarán al instanciar
    }

    void 
    visit(NewExpr *expr) override
    {
        // 1) buscar clase
        auto it = classes.find(expr->typeName);
        if (it == classes.end())
            throw std::runtime_error("Clase no declarada: " + expr->typeName);
        ClassDecl* cls = it->second;

        // 2) evaluar argumentos (simplemente los almacenamos por nombre)
        std::vector<Value> argVals;
        for (auto &a : expr->args)
        {
            a->accept(this);
            argVals.push_back(lastValue);
        }

        /* 3) crear marco de atributos ----------------------------- */
        auto fieldFrame = std::make_shared<EnvFrame>(nullptr);

        // (a) enlazar args como variables locales
        for (std::size_t i = 0; i < cls->attributes.size() && i < argVals.size(); ++i)
        {
            fieldFrame->locals[cls->attributes[i].first] = argVals[i];
        }

        // (b) evaluar inicializadores
        for (auto &attr : cls->attributes)
        {
            // si ya fue inicializado por args, sáltalo
            if (fieldFrame->locals.count(attr.first)) continue;

            attr.second->accept(this);
            fieldFrame->locals[attr.first] = lastValue;
        }

        // 4) construir objeto
        lastValue = Value(std::make_shared<ObjectValue>(cls, fieldFrame));
    }

    void 
    visit(SelfExpr *) override
    {
        if (!currentSelf.isObject())
            throw std::runtime_error("self fuera de método");
        lastValue = currentSelf;
    }

    void 
    visit(BaseExpr *) override
    {
        if (!currentClass || currentClass->parent == "Object")
            throw std::runtime_error("base fuera de contexto o sin superclase");
        auto parentIt = classes.find(currentClass->parent);
        if (parentIt == classes.end())
            throw std::runtime_error("Clase base no encontrada");
        lastValue = Value(std::make_shared<ObjectValue>(parentIt->second,
                                                    currentSelf.asObject()->fields));
    }

void visit(MemberAccessExpr *expr) override
    {
        expr->object->accept(this);          // evalúa el objeto
        if (!lastValue.isObject())
            throw std::runtime_error("Acceso a miembro sobre no-objeto");

        auto obj = lastValue.asObject();
        auto it = obj->fields->locals.find(expr->member);
        if (it == obj->fields->locals.end())
            throw std::runtime_error("Atributo inexistente: " + expr->member);

        lastValue = it->second;              // devuelve el atributo
    }

};

#endif