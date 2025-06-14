#include "type_inferer.hpp"
#include <stdexcept>

#include "../Type/type.hpp" // ahora tienes sólo UNA versión de find/unify
#include "../AST/ast.hpp"
#include <unordered_map>

// -- Constructor: entorno global vacío --
TypeInfererVisitor::TypeInfererVisitor()
    : env(std::make_shared<Scope<TypePtr>>(nullptr))
{
    // ========== built-ins ==========
    // print(x) → retorna un tipo genérico (TVAR)
    auto tvParam = Type::makeVar();
    auto tvRet = Type::makeVar();
    auto printType = Type::makeFunction({tvParam}, tvRet);
    env->declare("print", printType);

    // si deseas más funciones built-in, aquí las agregas:
    // env->declare("otraBuiltIn", otroTipo);
    // ==============================
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
    for (auto &arg : expr->args)
        arg->accept(this);
    auto fnType = env->lookup(expr->callee); // TypePtr :contentReference[oaicite:3]{index=3}
    for (size_t i = 0; i < expr->args.size(); ++i)
        unify(expr->args[i]->inferredType, fnType->params[i]);
    expr->inferredType = fnType->retType;
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
    // Crear tipo función con variables de tipo para parámetros y retorno
    std::vector<TypePtr> ps;
    for (size_t i = 0; i < stmt->params.size(); ++i)
        ps.push_back(Type::makeVar());
    auto retV = Type::makeVar();
    auto fT = Type::makeFunction(ps, retV);
    env->declare(stmt->name, fT);

    // Nuevo scope para cuerpo
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    for (size_t i = 0; i < stmt->params.size(); ++i)
        env->declare(stmt->params[i], ps[i]);
    stmt->body->accept(this);
    // unificar retorno
    auto *eb = dynamic_cast<ExprStmt *>(stmt->body.get());
    unify(retV, eb->expr->inferredType);
    env = parent;
}

void TypeInfererVisitor::visit(Program *prog)
{
    // Simplemente recorremos cada statement del programa
    for (auto &stmt : prog->stmts)
    {
        stmt->accept(this);
    }
}

void TypeInfererVisitor::visit(ClassDecl *decl)
{
    // 1) Inferir atributos como antes
    for (auto &attr : decl->attributes)
        attr.second->accept(this);

    // 2) Pre-declarar tipos de métodos en classMethods_
    for (auto &mStmt : decl->methods) {
        if (auto *f = dynamic_cast<FunctionDecl*>(mStmt.get())) {
            // Crear variables de tipo para parámetros y retorno
            std::vector<TypePtr> ps;
            for (size_t i = 0; i < f->params.size(); ++i)
                ps.push_back(Type::makeVar());
            auto retV = Type::makeVar();
            auto mType = Type::makeFunction(ps, retV);
            // Guardar en el mapa con clave "Clase"."método"
            classMethods_[decl->name][f->name] = mType;
        }
    }

    // 3) Inferir el cuerpo de cada método y unificar su retorno
    for (auto &mStmt : decl->methods) {
        if (auto *f = dynamic_cast<FunctionDecl*>(mStmt.get())) {
            // 3-a) crear nuevo scope para parámetros
            auto parentEnv = env;
            env = std::make_shared<Scope<TypePtr>>(parentEnv);
            // 3-b) declarar cada parámetro con su TVAR correspondiente
            auto &paramTypes = classMethods_[decl->name][f->name]->params;
            for (size_t i = 0; i < f->params.size(); ++i)
                env->declare(f->params[i], paramTypes[i]);
            // 3-c) inferir cuerpo
            f->body->accept(this);
            // 3-d) unificar retorno
            auto retV = classMethods_[decl->name][f->name]->retType;
            if (auto *eb = dynamic_cast<ExprStmt*>(f->body.get())) {
                unify(retV, eb->expr->inferredType);
            }
            // 3-e) restaurar scope
            env = parentEnv;
        }
    }
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

void TypeInfererVisitor::visit(MethodCallExpr *expr)
{
    // 1) Inferir tipo de receptor y argumentos
    expr->receiver->accept(this);
    for (auto &arg : expr->args)
        arg->accept(this);

    // 2) Comprobar que sea un tipo de clase
    auto recvT = find(expr->receiver->inferredType);
    if (recvT->kind != TypeKind::CLASS) {
        throw std::runtime_error("No se puede llamar método en tipo no-clase");
    }

    // 3) Buscar la firma del método en nuestro mapa
    auto cname = recvT->name;
    auto mit  = classMethods_.find(cname);
    if (mit == classMethods_.end()) {
        throw std::runtime_error("Clase no declarada: " + cname);
    }
    auto &mmap = mit->second;
    auto fit  = mmap.find(expr->methodName);
    if (fit == mmap.end()) {
        throw std::runtime_error(
          "Método '" + expr->methodName + "' no existe en clase " + cname);
    }
    auto mType = fit->second;

    // 4) Unificar tipos de argumentos
    for (size_t i = 0; i < expr->args.size(); ++i) {
        unify(expr->args[i]->inferredType, mType->params[i]);
    }

    // 5) Resultado de la llamada → tipo de retorno
    expr->inferredType = mType->retType;
}
