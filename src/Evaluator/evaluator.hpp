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
#include "../SemanticAnalysis/context.hpp" 
#include "../Errors/error_handler.hpp"
#include "../Errors/error_types/type_error.hpp"
#include "../Errors/error_types/name_error.hpp"
#include "../Errors/error_types/runtime_error.hpp"
#include "../Errors/error_types/value_error.hpp"

struct EvaluatorVisitor : StmtVisitor, ExprVisitor
{
    Value lastValue{0.0};
    // Usar sistema de contextos en lugar de EnvFrame
    std::shared_ptr<Context> env;
    // Mapa para almacenar valores de variables por contexto
    std::unordered_map<std::string, Value> variables;

    std::unordered_map<std::string, FunctionDecl *> functions;

    EvaluatorVisitor()
    {
        // Inicializar con un contexto "global" sin padre
        env = std::make_shared<Context>(nullptr);
    }

    // Programa: recorre stmt a stmt
    void
    visit(Program *p) override
    {
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

    // *** NUEVOS MÉTODOS PARA CLASES - IMPLEMENTACIÓN BÁSICA ***
    void visit(ClassDef *cls) override
    {
        // Por ahora, solo reportamos que encontramos una clase
        std::cout << "[INFO] Definición de clase encontrada: " << cls->name;
        if (!cls->parent.empty()) {
            std::cout << " (hereda de " << cls->parent << ")";
        }
        std::cout << std::endl;
        
        // TODO: Implementar sistema de tipos completo
        // Esto será parte del verificador de tipos que implementaremos después
    }

    void visit(AttrDef *attr) override
    {
        std::cout << "[INFO] Definición de atributo: " << attr->name << " : " << attr->type << std::endl;
        
        // TODO: Implementar gestión de atributos
        // Por ahora solo reportamos que encontramos el atributo
    }

    void visit(MethodDef *method) override
    {
        std::cout << "[INFO] Definición de método: " << method->name << std::endl;
        
        // TODO: Implementar gestión de métodos
        // Por ahora solo reportamos que encontramos el método
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
            auto oldVariables = variables;

            // Crear nuevo contexto hijo
            env = std::dynamic_pointer_cast<Context>(env->createChildContext());

            // Asignar parámetros como variables en el nuevo contexto
            for (size_t i = 0; i < f->params.size(); ++i)
            {
                env->define(f->params[i]);
                variables[f->params[i]] = args[i];
            }

            // Evaluar cuerpo
            f->body->accept(this);

            // Restaurar entorno
            env = oldEnv;
            variables = oldVariables;

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

    // Helper function to get variable value
    Value getVariable(const std::string& name)
    {
        if (variables.find(name) != variables.end())
        {
            return variables[name];
        }
        
        // Si no está en el mapa actual, buscar en variables built-in
        if (name == "PI") return Value(M_PI);
        if (name == "E") return Value(M_E);
        
        throw std::runtime_error("Variable no encontrada: " + name);
    }

    // Helper function to set variable value
    void setVariable(const std::string& name, const Value& value)
    {
        if (env->isDefined(name))
        {
            variables[name] = value;
        }
        else
        {
            throw std::runtime_error("Variable no definida: " + name);
        }
    }

    // for variable declarations
    void
    visit(VariableExpr *expr) override
    {
        if (!env->isDefined(expr->name))
        {
            ErrorManager::getInstance().report<NameError>(
                "Variable no definida: '" + expr->name + "'",
                Location(expr->getLine(), expr->getColumn())
            );
            return;
        }
        
        lastValue = getVariable(expr->name);
    }

    // let in expressions
    void
    visit(LetExpr *expr) override
    {
        // 1) Evaluar la expresión del inicializador
        expr->initializer->accept(this);
        Value initVal = lastValue;

        // 2) Guardar estado anterior
        auto oldEnv = env;
        auto oldVariables = variables;

        // 3) Crear nuevo contexto hijo
        env = std::dynamic_pointer_cast<Context>(env->createChildContext());

        // 4) Definir e insertar la variable
        env->define(expr->name);
        variables[expr->name] = initVal;

        // 5) Evaluar el cuerpo
        expr->body->accept(static_cast<StmtVisitor *>(this));
        Value result = lastValue;

        // 6) Restaurar estado anterior
        env = oldEnv;
        variables = oldVariables;

        lastValue = result;
    }

    // destructive assignment
    void
    visit(AssignExpr *expr) override
    {
        // Antes de asignar, evaluamos la expresión de la derecha:
        expr->value->accept(this);
        Value newVal = lastValue;

        // Verificar que exista en alguna parte:
        if (!env->isDefined(expr->name))
        {
            ErrorManager::getInstance().report<NameError>(
                "No se puede asignar a variable no declarada: '" + expr->name + "'",
                Location(expr->getLine(), expr->getColumn())
            );
            return;
        }
        
        setVariable(expr->name, newVal);
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
        // 1) Guardar estado anterior
        auto oldEnv = env;
        auto oldVariables = variables;

        // 2) Crear nuevo contexto hijo
        env = std::dynamic_pointer_cast<Context>(env->createChildContext());

        // 3) Evaluar cada sentencia dentro del bloque
        for (auto &stmt : b->stmts)
        {
            stmt->accept(this);
        }

        // 4) Restaurar estado anterior
        env = oldEnv;
        variables = oldVariables;
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

    // *** NUEVOS MÉTODOS PARA EXPRESIONES ORIENTADAS A OBJETOS - IMPLEMENTACIÓN BÁSICA ***
    void visit(NewExpr *expr) override
    {
        std::cout << "[INFO] Intento de crear instancia de clase: " << expr->className << std::endl;
        
        // TODO: Implementar creación real de objetos
        // Por ahora reportamos que se encontró la expresión
        lastValue = Value(0.0); // Valor temporal
    }

    void visit(AttributeAccessExpr *expr) override
    {
        std::cout << "[INFO] Acceso a atributo: " << expr->attribute << std::endl;
        
        // TODO: Implementar acceso real a atributos
        // Por ahora reportamos que se encontró la expresión
        expr->object->accept(this); // Evaluar el objeto
        lastValue = Value(0.0); // Valor temporal
    }

    void visit(MethodCallExpr *expr) override
    {
        std::cout << "[INFO] Llamada a método: " << expr->method << std::endl;
        
        // TODO: Implementar llamadas reales a métodos
        // Por ahora reportamos que se encontró la expresión
        expr->object->accept(this); // Evaluar el objeto
        
        // Evaluar argumentos
        for (auto &arg : expr->args) {
            arg->accept(this);
        }
        
        lastValue = Value(0.0); // Valor temporal
    }

    void visit(SelfExpr *expr) override
    {
        std::cout << "[INFO] Referencia a 'self'" << std::endl;
        
        // TODO: Implementar referencia real a self
        // Por ahora reportamos que se encontró la expresión
        lastValue = Value(0.0); // Valor temporal
    }
};

#endif