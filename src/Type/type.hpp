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

    // Métodos factory estáticos
    static std::shared_ptr<Type> makeNumber();
    static std::shared_ptr<Type> makeString();
    static std::shared_ptr<Type> makeBoolean();
    static std::shared_ptr<Type> makeVar();
    static std::shared_ptr<Type> makeFunction(const std::vector<std::shared_ptr<Type>>& params,
                                            std::shared_ptr<Type> ret);
    static std::shared_ptr<Type> makeObject(const std::string& className);
};

// Operaciones de unificación y LUB
std::shared_ptr<Type> find(std::shared_ptr<Type> t);
void unify(std::shared_ptr<Type> a, std::shared_ptr<Type> b);
std::shared_ptr<Type> lub(std::shared_ptr<Type> a, std::shared_ptr<Type> b);