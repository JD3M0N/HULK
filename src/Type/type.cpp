#include "type.hpp"
#include <stdexcept>

// find/unify/lub implementadas **aquí**
std::shared_ptr<Type> find(std::shared_ptr<Type> t)
{
    if (t->kind == TypeKind::TVAR && t->instance)
        return t->instance = find(t->instance);
    return t;
}

void unify(std::shared_ptr<Type> a, std::shared_ptr<Type> b)
{
    a = find(a);
    b = find(b);
    // if son el mismo objeto o el mismo tipo primitivo, OK
    if (a == b ||
        (a->kind == b->kind &&
         (a->kind == TypeKind::NUMBER ||
          a->kind == TypeKind::STRING ||
          a->kind == TypeKind::BOOLEAN)))
        return;
    if (a->kind == TypeKind::TVAR)
    {
        a->instance = b;
        return;
    }
    if (b->kind == TypeKind::TVAR)
    {
        b->instance = a;
        return;
    }
    // … resto de casos …
    throw std::runtime_error("Tipos incompatibles al unificar");
}

std::shared_ptr<Type> lub(std::shared_ptr<Type> A, std::shared_ptr<Type> B)
{
    A = find(A);
    B = find(B);
    // mismísima instancia o mismo tipo primitivo → devolvemos A
    if (A == B ||
        (A->kind == B->kind &&
         (A->kind == TypeKind::NUMBER ||
          A->kind == TypeKind::STRING ||
          A->kind == TypeKind::BOOLEAN)))
        return A;

    // clases nominales iguales
    if (A->kind == TypeKind::CLASS && B->kind == TypeKind::CLASS && A->name == B->name)
        return A;

    // en el resto de casos (herencia, mezcla de clases distintas) usamos Object
    return std::make_shared<Type>(TypeKind::CLASS, "Object");
}
