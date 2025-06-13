#pragma once
#include <string>
#include <vector>
#include <memory>

enum class TypeKind
{
    NUMBER,
    STRING,
    BOOLEAN,
    CLASS,
    FUNCTION,
    TVAR
};

// Representación de un tipo en HULK
struct Type
{
    TypeKind kind;
    std::string name;                          // para CLASS o protocolos
    std::vector<std::shared_ptr<Type>> params; // parámetros de función
    std::shared_ptr<Type> retType;             // tipo de retorno de función
    std::shared_ptr<Type> instance;            // enlace para TVAR

    Type(TypeKind k) : kind(k) {}
    Type(TypeKind k, const std::string &n) : kind(k), name(n) {}

    static std::shared_ptr<Type> makeNumber()
    {
        return std::make_shared<Type>(TypeKind::NUMBER);
    }
    static std::shared_ptr<Type> makeString()
    {
        return std::make_shared<Type>(TypeKind::STRING);
    }
    static std::shared_ptr<Type> makeBoolean()
    {
        return std::make_shared<Type>(TypeKind::BOOLEAN);
    }
    static std::shared_ptr<Type> makeVar()
    {
        return std::make_shared<Type>(TypeKind::TVAR);
    }
    static std::shared_ptr<Type> makeFunction(
        std::vector<std::shared_ptr<Type>> ps,
        std::shared_ptr<Type> ret)
    {
        auto t = std::make_shared<Type>(TypeKind::FUNCTION);
        t->params = std::move(ps);
        t->retType = std::move(ret);
        return t;
    }
};

// Operaciones de unificación y LUB
std::shared_ptr<Type> find(std::shared_ptr<Type> t);
void unify(std::shared_ptr<Type> a, std::shared_ptr<Type> b);
std::shared_ptr<Type> lub(std::shared_ptr<Type> a, std::shared_ptr<Type> b);