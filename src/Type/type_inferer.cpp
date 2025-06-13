#include "type_inferer.hpp"
#include <stdexcept>
#include <iostream>
#include "../Type/type.hpp"
#include "../AST/ast.hpp"

// Funciones auxiliares para visualización
void TypeInfererVisitor::printIndent()
{
    for (int i = 0; i < indentLevel; ++i)
        std::cout << "    ";
}

void TypeInfererVisitor::printTypeInfo(const std::string& nodeType, const std::string& extra)
{
    printIndent();
    std::cout << "|_ " << nodeType;
    if (!extra.empty())
        std::cout << ": " << extra;
    std::cout << std::endl;
}

void TypeInfererVisitor::printInferredType(TypePtr type)
{
    printIndent();
    std::cout << "  -> Tipo inferido: " << typeToString(type) << std::endl;
}

std::string TypeInfererVisitor::typeToString(TypePtr type)
{
    if (!type) return "NULL";
    
    type = find(type); // Resolver variables de tipo
    
    switch (type->kind)
    {
        case TypeKind::NUMBER: return "Number";
        case TypeKind::STRING: return "String";
        case TypeKind::BOOLEAN: return "Boolean";
        case TypeKind::CLASS: return "Class(" + type->name + ")";
        case TypeKind::FUNCTION:
        {
            std::string result = "Function(";
            for (size_t i = 0; i < type->params.size(); ++i)
            {
                if (i > 0) result += ", ";
                result += typeToString(type->params[i]);
            }
            result += ") -> " + typeToString(type->retType);
            return result;
        }
        case TypeKind::TVAR:
            return "TVar@" + std::to_string(reinterpret_cast<uintptr_t>(type.get()));
    }
    return "Unknown";
}

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
    printTypeInfo("NumberExpr", std::to_string(expr->value));
    indentLevel++;
    
    expr->inferredType = Type::makeNumber();
    printInferredType(expr->inferredType);
    
    indentLevel--;
}

void TypeInfererVisitor::visit(StringExpr *expr)
{
    printTypeInfo("StringExpr", "\"" + expr->value + "\"");
    indentLevel++;
    
    expr->inferredType = Type::makeString();
    printInferredType(expr->inferredType);
    
    indentLevel--;
}

void TypeInfererVisitor::visit(BooleanExpr *expr)
{
    printTypeInfo("BooleanExpr", expr->value ? "true" : "false");
    indentLevel++;
    
    expr->inferredType = Type::makeBoolean();
    printInferredType(expr->inferredType);
    
    indentLevel--;
}

void TypeInfererVisitor::visit(UnaryExpr *expr)
{
    printTypeInfo("UnaryExpr", "-");
    indentLevel++;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando operando:" << std::endl;
    expr->operand->accept(this);
    
    expr->inferredType = expr->operand->inferredType;
    printInferredType(expr->inferredType);
    
    indentLevel--;
}

void TypeInfererVisitor::visit(BinaryExpr *expr)
{
    std::string opStr;
    switch(expr->op) {
        case BinaryExpr::Op::OP_ADD: opStr = "+"; break;
        case BinaryExpr::Op::OP_SUB: opStr = "-"; break;
        case BinaryExpr::Op::OP_MUL: opStr = "*"; break;
        case BinaryExpr::Op::OP_DIV: opStr = "/"; break;
        case BinaryExpr::Op::OP_POW: opStr = "^"; break;
        case BinaryExpr::Op::OP_MOD: opStr = "%"; break;
        case BinaryExpr::Op::OP_CONCAT: opStr = "@"; break;
        case BinaryExpr::Op::OP_AND: opStr = "&&"; break;
        case BinaryExpr::Op::OP_OR: opStr = "||"; break;
        default: opStr = "comparación"; break;
    }
    
    printTypeInfo("BinaryExpr", opStr);
    indentLevel++;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando lado izquierdo:" << std::endl;
    expr->left->accept(this);
    auto L = expr->left->inferredType;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando lado derecho:" << std::endl;
    expr->right->accept(this);
    auto R = expr->right->inferredType;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Unificando tipos: " << typeToString(L) << " con " << typeToString(R) << std::endl;
    
    using Op = BinaryExpr::Op;
    if (expr->op == Op::OP_ADD || expr->op == Op::OP_SUB ||
        expr->op == Op::OP_MUL || expr->op == Op::OP_DIV ||
        expr->op == Op::OP_POW || expr->op == Op::OP_MOD)
    {
        try {
            unify(L, R);
            expr->inferredType = Type::makeNumber();
            printIndent();
            std::cout << " Unificación exitosa para operación aritmética" << std::endl;
        } catch (const std::exception& e) {
            printIndent();
            std::cout << " Error en unificación: " << e.what() << std::endl;
            throw;
        }
    }
    else if (expr->op == Op::OP_AND || expr->op == Op::OP_OR)
    {
        try {
            unify(L, Type::makeBoolean());
            unify(R, Type::makeBoolean());
            expr->inferredType = Type::makeBoolean();
            printIndent();
            std::cout << " Unificación exitosa para operación lógica" << std::endl;
        } catch (const std::exception& e) {
            printIndent();
            std::cout << " Error en unificación lógica: " << e.what() << std::endl;
            throw;
        }
    }
    else if (expr->op == Op::OP_CONCAT)
    {
        try {
            unify(L, Type::makeString());
            unify(R, Type::makeString());
            expr->inferredType = Type::makeString();
            printIndent();
            std::cout << " Unificación exitosa para concatenación" << std::endl;
        } catch (const std::exception& e) {
            printIndent();
            std::cout << " Error en concatenación: " << e.what() << std::endl;
            throw;
        }
    }
    else
    {
        try {
            unify(L, R);
            expr->inferredType = Type::makeBoolean();
            printIndent();
            std::cout << " Unificación exitosa para comparación" << std::endl;
        } catch (const std::exception& e) {
            printIndent();
            std::cout << " Error en comparación: " << e.what() << std::endl;
            throw;
        }
    }
    
    printInferredType(expr->inferredType);
    indentLevel--;
}

void TypeInfererVisitor::visit(CallExpr *expr)
{
    printTypeInfo("CallExpr", expr->callee);
    indentLevel++;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando argumentos:" << std::endl;
    indentLevel++;
    for (auto &arg : expr->args) {
        arg->accept(this);
    }
    indentLevel--;
    
    printIndent();
    std::cout << "Buscando funcion: " << expr->callee << std::endl;
    
    try {
        TypePtr fnType;
        
        // Manejar funciones built-in con tipos polimórficos
        if (expr->callee == "print") {
            printIndent();
            std::cout << "Función built-in polimórfica: print" << std::endl;
            
            // Crear variables de tipo frescas para cada llamada
            auto freshParam = Type::makeVar();
            auto freshRet = Type::makeVar();
            fnType = Type::makeFunction({freshParam}, freshRet);
            
            printIndent();
            std::cout << "Tipo polimórfico creado: " << typeToString(fnType) << std::endl;
        } else {
            // Funciones definidas por el usuario
            fnType = env->lookup(expr->callee);
            printIndent();
            std::cout << "Tipo de funcion encontrado: " << typeToString(fnType) << std::endl;
        }
        
        // Verificar número de argumentos
        if (expr->args.size() != fnType->params.size()) {
            throw std::runtime_error("Número incorrecto de argumentos para " + expr->callee);
        }
        
        printIndent();
        std::cout << "Unificando argumentos con parametros:" << std::endl;
        indentLevel++;
        for (size_t i = 0; i < expr->args.size(); ++i) {
            printIndent();
            std::cout << "Arg[" << i << "]: " << typeToString(expr->args[i]->inferredType) 
                      << " con Param[" << i << "]: " << typeToString(fnType->params[i]) << std::endl;
            unify(expr->args[i]->inferredType, fnType->params[i]);
            printIndent();
            std::cout << "✓ Unificacion exitosa" << std::endl;
        }
        indentLevel--;
        
        expr->inferredType = fnType->retType;
        printInferredType(expr->inferredType);
    } catch (const std::exception& e) {
        printIndent();
        std::cout << "✗ Error en llamada a funcion: " << e.what() << std::endl;
        throw;
    }
    
    indentLevel--;
}

void TypeInfererVisitor::visit(VariableExpr *expr)
{
    printTypeInfo("VariableExpr", expr->name);
    indentLevel++;
    
    try {
        expr->inferredType = env->lookup(expr->name);
        printIndent();
        std::cout << "Variable encontrada en entorno" << std::endl;
        printInferredType(expr->inferredType);
    } catch (const std::exception& e) {
        printIndent();
        std::cout << " Error: variable no encontrada: " << e.what() << std::endl;
        throw;
    }
    
    indentLevel--;
}

void TypeInfererVisitor::visit(LetExpr *expr)
{
    printTypeInfo("LetExpr", expr->name);
    indentLevel++;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando inicializador:" << std::endl;
    expr->initializer->accept(this);
    auto initT = expr->initializer->inferredType;
    
    printIndent();
    std::cout << "Creando nuevo scope para variable: " << expr->name << std::endl;
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    env->declare(expr->name, initT);
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando cuerpo:" << std::endl;
    expr->body->accept(this);
    
    expr->inferredType = dynamic_cast<ExprStmt *>(expr->body.get())
                             ? static_cast<ExprStmt *>(expr->body.get())->expr->inferredType
                             : nullptr;
                             
    printIndent();
    std::cout << "Restaurando scope anterior" << std::endl;
    env = parent;
    
    printInferredType(expr->inferredType);
    indentLevel--;
}

void TypeInfererVisitor::visit(AssignExpr *expr)
{
    printTypeInfo("AssignExpr", expr->name);
    indentLevel++;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando valor a asignar:" << std::endl;
    expr->value->accept(this);
    
    printIndent();
    std::cout << "Verificando existencia de variable: " << expr->name << std::endl;
    try {
        env->lookup(expr->name);
        printIndent();
        std::cout << " Variable existe" << std::endl;
    } catch (const std::exception& e) {
        printIndent();
        std::cout << " Error: " << e.what() << std::endl;
        throw;
    }
    
    expr->inferredType = expr->value->inferredType;
    printInferredType(expr->inferredType);
    
    indentLevel--;
}

void TypeInfererVisitor::visit(IfExpr *expr)
{
    printTypeInfo("IfExpr");
    indentLevel++;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando condicion:" << std::endl;
    expr->condition->accept(this);
    
    printIndent();
    std::cout << "Unificando condicion con Boolean" << std::endl;
    try {
        unify(expr->condition->inferredType, Type::makeBoolean());
        printIndent();
        std::cout << " Condicion es booleana" << std::endl;
    } catch (const std::exception& e) {
        printIndent();
        std::cout << " Error: condicion no booleana: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando rama then:" << std::endl;
    expr->thenBranch->accept(this);
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando rama else:" << std::endl;
    expr->elseBranch->accept(this);
    
    printIndent();
    std::cout << "Calculando LUB de ramas:" << std::endl;
    printIndent();
    std::cout << "Then: " << typeToString(expr->thenBranch->inferredType) << std::endl;
    printIndent();
    std::cout << "Else: " << typeToString(expr->elseBranch->inferredType) << std::endl;
    
    expr->inferredType = lub(expr->thenBranch->inferredType, expr->elseBranch->inferredType);
    printInferredType(expr->inferredType);
    
    indentLevel--;
}

void TypeInfererVisitor::visit(ExprBlock *expr)
{
    printTypeInfo("ExprBlock");
    indentLevel++;
    
    printIndent();
    std::cout << "Creando nuevo scope para bloque" << std::endl;
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    
    for (auto &s : expr->stmts) {
        std::cout << std::endl;
        s->accept(this);
    }
    
    if (!expr->stmts.empty()) {
        auto *last = dynamic_cast<ExprStmt *>(expr->stmts.back().get());
        expr->inferredType = last->expr->inferredType;
    }
    
    printIndent();
    std::cout << "Restaurando scope anterior" << std::endl;
    env = parent;
    
    printInferredType(expr->inferredType);
    indentLevel--;
}

void TypeInfererVisitor::visit(WhileExpr *expr)
{
    printTypeInfo("WhileExpr");
    indentLevel++;
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando condicion:" << std::endl;
    expr->condition->accept(this);
    
    printIndent();
    std::cout << "Unificando condicion con Boolean" << std::endl;
    try {
        unify(expr->condition->inferredType, Type::makeBoolean());
        printIndent();
        std::cout << " Condicion es booleana" << std::endl;
    } catch (const std::exception& e) {
        printIndent();
        std::cout << " Error: condicion no booleana: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando cuerpo:" << std::endl;
    expr->body->accept(this);
    
    auto *last = dynamic_cast<ExprStmt *>(expr->body.get());
    expr->inferredType = last ? last->expr->inferredType : Type::makeBoolean();
    printInferredType(expr->inferredType);
    
    indentLevel--;
}

// -- StmtVisitor --
void TypeInfererVisitor::visit(ExprStmt *stmt)
{
    printTypeInfo("ExprStmt");
    indentLevel++;
    stmt->expr->accept(this);
    indentLevel--;
}

void TypeInfererVisitor::visit(FunctionDecl *stmt)
{
    printTypeInfo("FunctionDecl", stmt->name);
    indentLevel++;
    
    std::vector<TypePtr> ps;
    for (size_t i = 0; i < stmt->params.size(); ++i) {
        ps.push_back(Type::makeVar());
        printIndent();
        std::cout << "Param[" << i << "]: " << stmt->params[i] << " -> " << typeToString(ps[i]) << std::endl;
    }
    
    auto retV = Type::makeVar();
    auto fT = Type::makeFunction(ps, retV);
    
    printIndent();
    std::cout << "Declarando funcion: " << typeToString(fT) << std::endl;
    env->declare(stmt->name, fT);
    
    printIndent();
    std::cout << "Creando nuevo scope para cuerpo de funcion" << std::endl;
    auto parent = env;
    env = std::make_shared<Scope<TypePtr>>(parent);
    
    for (size_t i = 0; i < stmt->params.size(); ++i) {
        env->declare(stmt->params[i], ps[i]);
    }
    
    std::cout << std::endl;
    printIndent();
    std::cout << "Procesando cuerpo de funcion:" << std::endl;
    stmt->body->accept(this);
    
    auto *eb = dynamic_cast<ExprStmt *>(stmt->body.get());
    if (eb) {
        printIndent();
        std::cout << "Unificando tipo de retorno:" << std::endl;
        printIndent();
        std::cout << "Esperado: " << typeToString(retV) << std::endl;
        printIndent();
        std::cout << "Real: " << typeToString(eb->expr->inferredType) << std::endl;
        
        try {
            unify(retV, eb->expr->inferredType);
            printIndent();
            std::cout << " Unificacion de retorno exitosa" << std::endl;
        } catch (const std::exception& e) {
            printIndent();
            std::cout << " Error en tipo de retorno: " << e.what() << std::endl;
            throw;
        }
    }
    
    printIndent();
    std::cout << "Restaurando scope anterior" << std::endl;
    env = parent;
    
    indentLevel--;
}

void TypeInfererVisitor::visit(Program *prog)
{
    std::cout << "\n=== ARBOL DE INFERENCIA DE TIPOS ===" << std::endl;
    printTypeInfo("Program");
    indentLevel++;
    
    for (auto &stmt : prog->stmts) {
        std::cout << std::endl;
        stmt->accept(this);
    }
    
    indentLevel--;
    std::cout << "\n=== FIN INFERENCIA DE TIPOS ===\n" << std::endl;
}
