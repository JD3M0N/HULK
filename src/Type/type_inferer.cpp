#include "type_inferer.hpp"
#include <stdexcept>
#include <iostream>

#include "../Type/type.hpp" // ahora tienes sólo UNA versión de find/unify
#include "../AST/ast.hpp"

// -- Constructor: entorno global vacío --
TypeInfererVisitor::TypeInfererVisitor()
    : env(std::make_shared<Scope<TypePtr>>(nullptr))
{
    std::cout << "=== INICIALIZANDO INFERENCIA DE TIPOS ===" << std::endl;

    // NO registrar print aquí - será manejado en cada CallExpr
    std::cout << "Entorno base inicializado" << std::endl;
}

// -- ExprVisitor --
void TypeInfererVisitor::visit(NumberExpr *expr)
{
    expr->inferredType = Type::makeNumber();
}
void TypeInfererVisitor::visit(StringExpr *expr)
{
    expr->inferredType = Type::makeString();
}
void TypeInfererVisitor::visit(BooleanExpr *expr)
{
    expr->inferredType = Type::makeBoolean();
}
void TypeInfererVisitor::visit(UnaryExpr *expr)
{
    expr->operand->accept(this);
    expr->inferredType = expr->operand->inferredType;
}
void TypeInfererVisitor::visit(BinaryExpr *expr)
{
    expr->left->accept(this);
    expr->right->accept(this);
    auto L = expr->left->inferredType;
    auto R = expr->right->inferredType;

    using Op = BinaryExpr::Op;
    if (expr->op == Op::OP_ADD || expr->op == Op::OP_SUB ||
        expr->op == Op::OP_MUL || expr->op == Op::OP_DIV ||
        expr->op == Op::OP_POW || expr->op == Op::OP_MOD)
    {
        unify(L, R);
        expr->inferredType = Type::makeNumber();
    }
    else if (expr->op == Op::OP_AND || expr->op == Op::OP_OR)
    {
        unify(L, Type::makeBoolean());
        unify(R, Type::makeBoolean());
        expr->inferredType = Type::makeBoolean();
    }
    else if (expr->op == Op::OP_CONCAT)
    {
        unify(L, Type::makeString());
        unify(R, Type::makeString());
        expr->inferredType = Type::makeString();
    }
    else
    {
        // comparaciones -> Boolean
        unify(L, R);
        expr->inferredType = Type::makeBoolean();
    }
}
void TypeInfererVisitor::visit(CallExpr *expr)
{
    std::cout << "DEBUG: Iniciando visit(CallExpr) para función/método: " << expr->callee << std::endl;
    std::cout << "DEBUG: CallExpr tiene " << expr->args.size() << " argumentos" << std::endl;

    for (size_t i = 0; i < expr->args.size(); ++i)
    {
        std::cout << "DEBUG: Procesando argumento " << i << " de CallExpr " << expr->callee << std::endl;
        if (!expr->args[i])
        {
            std::cout << "ERROR: Argumento " << i << " es nulo!" << std::endl;
            continue;
        }

        try
        {
            expr->args[i]->accept(this);
            std::cout << "DEBUG: Argumento " << i << " procesado exitosamente" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "ERROR: Excepción en argumento " << i << ": " << e.what() << std::endl;
            throw;
        }
    }

    TypePtr fnType;

    if (expr->callee == "print")
    {
        std::cout << "DEBUG: CallExpr es función print" << std::endl;
        auto freshParam = Type::makeVar();
        auto freshRet = Type::makeVar();
        fnType = Type::makeFunction({freshParam}, freshRet);
    }
    else
    {
        // Detectar si es una llamada a método
        bool isMethodCall = false;
        if (!expr->args.empty())
        {
            // Si tenemos argumentos y el primer argumento es una expresión compleja,
            // probablemente es una llamada a método
            if (auto *firstArg = expr->args[0].get())
            {
                if (!dynamic_cast<VariableExpr *>(firstArg))
                {
                    isMethodCall = true;
                }
            }
        }

        std::cout << "DEBUG: CallExpr " << expr->callee << " es " << (isMethodCall ? "llamada a método" : "llamada a función") << std::endl;

        if (isMethodCall)
        {
            // Es una llamada a método - no buscar en scope global
            // Asumir que es válida y crear un tipo de función genérico
            std::cout << "DEBUG: Creando tipo genérico para método " << expr->callee << std::endl;
            std::vector<TypePtr> paramTypes;
            for (auto &arg : expr->args)
            {
                if (arg && arg->inferredType)
                {
                    paramTypes.push_back(arg->inferredType);
                }
                else
                {
                    std::cout << "WARNING: Argumento sin tipo inferido, usando tipo variable" << std::endl;
                    paramTypes.push_back(Type::makeVar());
                }
            }
            auto freshRet = Type::makeVar();
            fnType = Type::makeFunction(paramTypes, freshRet);
        }
        else
        {
            // Llamada a función regular - buscar en scope
            std::cout << "DEBUG: Buscando función " << expr->callee << " en scope" << std::endl;
            try
            {
                fnType = env->lookup(expr->callee);
                std::cout << "DEBUG: Función " << expr->callee << " encontrada en scope" << std::endl;
            }
            catch (const std::exception &e)
            {
                // Si no se encuentra, crear un tipo genérico
                std::cout << "DEBUG: Función " << expr->callee << " no encontrada, creando tipo genérico. Error: " << e.what() << std::endl;
                std::vector<TypePtr> paramTypes;
                for (auto &arg : expr->args)
                {
                    if (arg && arg->inferredType)
                    {
                        paramTypes.push_back(arg->inferredType);
                    }
                    else
                    {
                        paramTypes.push_back(Type::makeVar());
                    }
                }
                auto freshRet = Type::makeVar();
                fnType = Type::makeFunction(paramTypes, freshRet);
            }
        }
    }

    // Verificar que fnType sea válido y tenga el número correcto de parámetros
    if (fnType)
    {
        std::cout << "DEBUG: fnType válido para " << expr->callee << ", verificando parámetros" << std::endl;
        std::cout << "DEBUG: fnType tiene " << fnType->params.size() << " parámetros, expr tiene " << expr->args.size() << " argumentos" << std::endl;

        if (fnType->params.size() == expr->args.size())
        {
            for (size_t i = 0; i < expr->args.size(); ++i)
            {
                if (expr->args[i] && expr->args[i]->inferredType && fnType->params[i])
                {
                    std::cout << "DEBUG: Unificando argumento " << i << std::endl;
                    try
                    {
                        unify(expr->args[i]->inferredType, fnType->params[i]);
                    }
                    catch (const std::exception &e)
                    {
                        std::cout << "ERROR: Error al unificar argumento " << i << ": " << e.what() << std::endl;
                        throw;
                    }
                }
                else
                {
                    std::cout << "WARNING: Problema con argumento " << i << " o tipo de parámetro" << std::endl;
                }
            }

            if (fnType->retType)
            {
                expr->inferredType = fnType->retType;
                std::cout << "DEBUG: Tipo de retorno asignado para " << expr->callee << std::endl;
            }
            else
            {
                std::cout << "WARNING: fnType->retType es nulo, usando tipo variable" << std::endl;
                expr->inferredType = Type::makeVar();
            }
        }
        else
        {
            std::cout << "WARNING: Número de parámetros no coincide para " << expr->callee << std::endl;
            expr->inferredType = Type::makeVar();
        }
    }
    else
    {
        // Tipo de retorno genérico si hay problemas
        std::cout << "WARNING: fnType es nulo para " << expr->callee << ", usando tipo variable" << std::endl;
        expr->inferredType = Type::makeVar();
    }

    std::cout << "DEBUG: CallExpr " << expr->callee << " completado exitosamente" << std::endl;
}
void TypeInfererVisitor::visit(VariableExpr *expr)
{
    expr->inferredType = env->lookup(expr->name);
}
void TypeInfererVisitor::visit(LetExpr *expr)
{
    expr->initializer->accept(this);
    auto initT = expr->initializer->inferredType;
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    env->declare(expr->name, initT);
    expr->body->accept(this);
    expr->inferredType = dynamic_cast<ExprStmt *>(expr->body.get())
                             ? static_cast<ExprStmt *>(expr->body.get())->expr->inferredType
                             : nullptr;
    env = parent;
}
void TypeInfererVisitor::visit(AssignExpr *expr)
{
    expr->value->accept(this);
    env->lookup(expr->name); // verificar existencia
    expr->inferredType = expr->value->inferredType;
}
void TypeInfererVisitor::visit(IfExpr *expr)
{
    expr->condition->accept(this);
    unify(expr->condition->inferredType, Type::makeBoolean());
    expr->thenBranch->accept(this);
    expr->elseBranch->accept(this);
    expr->inferredType = lub(
        expr->thenBranch->inferredType,
        expr->elseBranch->inferredType);
}
void TypeInfererVisitor::visit(ExprBlock *expr)
{
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    for (auto &s : expr->stmts)
        s->accept(this);
    // el tipo es el de la última expresión del bloque
    if (!expr->stmts.empty())
    {
        auto *last = dynamic_cast<ExprStmt *>(expr->stmts.back().get());
        expr->inferredType = last->expr->inferredType;
    }
    env = parent;
}
void TypeInfererVisitor::visit(WhileExpr *expr)
{
    expr->condition->accept(this);
    unify(expr->condition->inferredType, Type::makeBoolean());
    expr->body->accept(this);
    // devuelve el tipo de la última iteración
    auto *last = dynamic_cast<ExprStmt *>(expr->body.get());
    expr->inferredType = last ? last->expr->inferredType : Type::makeBoolean();
}

// -- StmtVisitor --
void TypeInfererVisitor::visit(ExprStmt *stmt)
{
    stmt->expr->accept(this);
}
void TypeInfererVisitor::visit(FunctionDecl *stmt)
{
    std::cout << "DEBUG: Iniciando visit(FunctionDecl) para función: " << stmt->name << std::endl;

    if (!stmt)
    {
        std::cout << "ERROR: FunctionDecl es nulo!" << std::endl;
        return;
    }

    std::cout << "DEBUG: Función " << stmt->name << " tiene " << stmt->params.size() << " parámetros" << std::endl;

    // Crear tipo función con variables de tipo para parámetros y retorno
    std::vector<TypePtr> ps;
    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        std::cout << "DEBUG: Creando tipo para parámetro " << i << ": " << stmt->params[i] << std::endl;
        ps.push_back(Type::makeVar());
    }
    auto retV = Type::makeVar();
    auto fT = Type::makeFunction(ps, retV);

    std::cout << "DEBUG: Declarando función " << stmt->name << " en scope" << std::endl;
    env->declare(stmt->name, fT);

    // Nuevo scope para cuerpo
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);

    std::cout << "DEBUG: Creado scope para cuerpo de función " << stmt->name << std::endl;

    for (size_t i = 0; i < stmt->params.size(); ++i)
    {
        std::cout << "DEBUG: Declarando parámetro " << stmt->params[i] << " en scope de función" << std::endl;
        env->declare(stmt->params[i], ps[i]);
    }

    std::cout << "DEBUG: Procesando cuerpo de función " << stmt->name << std::endl;

    if (!stmt->body)
    {
        std::cout << "ERROR: Cuerpo de función " << stmt->name << " es nulo!" << std::endl;
        env = parent;
        return;
    }

    try
    {
        stmt->body->accept(this);
        std::cout << "DEBUG: Cuerpo de función " << stmt->name << " procesado exitosamente" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "ERROR: Excepción en cuerpo de función " << stmt->name << ": " << e.what() << std::endl;
        env = parent;
        throw;
    }

    // unificar retorno
    auto *eb = dynamic_cast<ExprStmt *>(stmt->body.get());
    if (eb && eb->expr)
    {
        std::cout << "DEBUG: Unificando tipo de retorno de función " << stmt->name << std::endl;
        unify(retV, eb->expr->inferredType);
    }
    else
    {
        std::cout << "WARNING: No se pudo obtener tipo de retorno para función " << stmt->name << std::endl;
    }

    env = parent;
    std::cout << "DEBUG: FunctionDecl " << stmt->name << " completada exitosamente" << std::endl;
}

void TypeInfererVisitor::visit(Program *prog)
{
    std::cout << "DEBUG: Iniciando visit(Program) con " << prog->stmts.size() << " statements" << std::endl;

    // Simplemente recorremos cada statement del programa
    for (size_t i = 0; i < prog->stmts.size(); ++i)
    {
        std::cout << "DEBUG: Procesando statement " << i << std::endl;

        if (!prog->stmts[i])
        {
            std::cout << "ERROR: Statement " << i << " es nulo!" << std::endl;
            continue;
        }

        // Verificar tipo de statement
        if (dynamic_cast<ClassDecl *>(prog->stmts[i].get()))
        {
            std::cout << "DEBUG: Statement " << i << " es ClassDecl" << std::endl;
        }
        else if (dynamic_cast<FunctionDecl *>(prog->stmts[i].get()))
        {
            std::cout << "DEBUG: Statement " << i << " es FunctionDecl" << std::endl;
        }
        else if (dynamic_cast<ExprStmt *>(prog->stmts[i].get()))
        {
            std::cout << "DEBUG: Statement " << i << " es ExprStmt" << std::endl;
        }
        else
        {
            std::cout << "DEBUG: Statement " << i << " es de tipo desconocido" << std::endl;
        }

        try
        {
            prog->stmts[i]->accept(this);
            std::cout << "DEBUG: Statement " << i << " procesado exitosamente" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "ERROR: Excepción en statement " << i << ": " << e.what() << std::endl;
            throw;
        }
    }
    std::cout << "DEBUG: Program completado exitosamente" << std::endl;
}

void TypeInfererVisitor::visit(ClassDecl *decl)
{
    std::cout << "DEBUG: Iniciando visit(ClassDecl) para clase: " << decl->name << std::endl;

    if (!decl)
    {
        std::cout << "ERROR: ClassDecl es nulo!" << std::endl;
        return;
    }

    std::cout << "DEBUG: Clase " << decl->name << " tiene " << decl->attributes.size() << " atributos y " << decl->methods.size() << " métodos" << std::endl;

    // Inferimos cada inicializador de atributo:
    for (size_t i = 0; i < decl->attributes.size(); ++i)
    {
        std::cout << "DEBUG: Procesando atributo " << i << " de clase " << decl->name << std::endl;
        if (decl->attributes[i].second)
        {
            decl->attributes[i].second->accept(this);
        }
        else
        {
            std::cout << "WARNING: Atributo " << i << " tiene inicializador nulo" << std::endl;
        }
    }

    // Para los métodos, crear un scope separado para cada clase
    // para evitar colisiones de nombres entre métodos de diferentes clases
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);

    std::cout << "DEBUG: Creado scope separado para clase " << decl->name << std::endl;

    // Inferimos cada método (FunctionDecl*) en el scope de la clase
    for (size_t i = 0; i < decl->methods.size(); ++i)
    {
        std::cout << "DEBUG: Procesando método " << i << " de clase " << decl->name << std::endl;

        if (!decl->methods[i])
        {
            std::cout << "ERROR: Método " << i << " es nulo!" << std::endl;
            continue;
        }

        try
        {
            decl->methods[i]->accept(this);
            std::cout << "DEBUG: Método " << i << " de clase " << decl->name << " procesado exitosamente" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << "ERROR: Excepción en método " << i << " de clase " << decl->name << ": " << e.what() << std::endl;
            throw;
        }
    }

    // Restaurar el scope padre
    env = parent;
    std::cout << "DEBUG: Scope restaurado para clase " << decl->name << std::endl;

    // No devolvemos nada; las declaraciones de clase no son expresiones
    std::cout << "DEBUG: ClassDecl " << decl->name << " completada exitosamente" << std::endl;
}

void TypeInfererVisitor::visit(NewExpr *expr)
{
    // Inferimos los argumentos
    for (auto &arg : expr->args)
    {
        arg->accept(this);
    }
    // El tipo resultante de `new T(...)` es la clase T
    expr->inferredType = std::make_shared<Type>(TypeKind::CLASS, expr->typeName);
}

void TypeInfererVisitor::visit(SelfExpr *expr)
{
    expr->inferredType = Type::makeVar();
}

void TypeInfererVisitor::visit(BaseExpr *expr)
{
    expr->inferredType = Type::makeVar();
}
void TypeInfererVisitor::visit(MemberAccessExpr *expr)
{
    // Inferimos el objeto, y luego hacemos stub para el miembro:
    expr->object->accept(this);
    expr->inferredType = Type::makeVar();
}

void TypeInfererVisitor::visit(MemberAssignExpr *expr)
{
    expr->object->accept(this);
    expr->value->accept(this);
    // Stub: unificamos el tipo del valor con el tipo declarado del atributo,
    // o simplemente propagamos el tipo de expr->value:
    expr->inferredType = expr->value->inferredType;
}
