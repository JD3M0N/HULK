#include "type.hpp"
#include <stdexcept>

// Implementación de find/unify/lub para el sistema de tipos

// find: Resuelve el tipo real de una variable de tipo (TVAR), siguiendo enlaces de unificación.
std::shared_ptr<Type> find(std::shared_ptr<Type> t)
{
    // Si es una variable de tipo (TVAR) y ya está unificada, seguimos el enlace recursivamente
    if (t->kind == TypeKind::TVAR && t->instance)
        return t->instance = find(t->instance); // Path compression
    return t; // Si no, devolvemos el tipo tal cual
}

// unify: Unifica dos tipos, enlazando variables de tipo si es necesario
void unify(std::shared_ptr<Type> a, std::shared_ptr<Type> b)
{
    a = find(a); // Resuelve el tipo real de 'a'
    b = find(b); // Resuelve el tipo real de 'b'
    // Si son exactamente el mismo objeto o el mismo tipo primitivo, no hay nada que hacer
    if (a == b ||
        (a->kind == b->kind &&
         (a->kind == TypeKind::NUMBER ||
          a->kind == TypeKind::STRING ||
          a->kind == TypeKind::BOOLEAN)))
        return;
    // Si 'a' es una variable de tipo, la enlazamos a 'b'
    if (a->kind == TypeKind::TVAR)
    {
        a->instance = b;
        return;
    }
    // Si 'b' es una variable de tipo, la enlazamos a 'a'
    if (b->kind == TypeKind::TVAR)
    {
        b->instance = a;
        return;
    }
    // Si no se pueden unificar, lanzamos un error
    // (Aquí se podrían agregar más reglas para funciones, clases, etc.)
    throw std::runtime_error("Tipos incompatibles al unificar");
}

// lub: Calcula el 'least upper bound' (tipo más general) entre dos tipos
std::shared_ptr<Type> lub(std::shared_ptr<Type> A, std::shared_ptr<Type> B)
{
    A = find(A); // Resuelve el tipo real de 'A'
    B = find(B); // Resuelve el tipo real de 'B'
    // Si son la misma instancia o el mismo tipo primitivo, devolvemos 'A'
    if (A == B ||
        (A->kind == B->kind &&
         (A->kind == TypeKind::NUMBER ||
          A->kind == TypeKind::STRING ||
          A->kind == TypeKind::BOOLEAN)))
        return A;

    // Si son clases nominales iguales, devolvemos 'A'
    if (A->kind == TypeKind::CLASS && B->kind == TypeKind::CLASS && A->name == B->name)
        return A;

    // En el resto de casos (herencia, mezcla de clases distintas), devolvemos 'Object' como tipo común
    return std::make_shared<Type>(TypeKind::CLASS, "Object");
}
