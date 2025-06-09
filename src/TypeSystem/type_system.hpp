#ifndef TYPE_CONTEXT_HPP
#define TYPE_CONTEXT_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "context.hpp"
#include "../TypeSystem/type_system.hpp"

// Forward declarations
class IType;

// Interface extendida del contexto que integra el sistema de tipos
class ITypeContext : public IContext {
public:
    virtual ~ITypeContext() = default;
    
    // Métodos heredados de IContext
    // (se implementarán delegando al contexto base)
    
    // Nuevos métodos para el sistema de tipos
    virtual std::shared_ptr<IType> getType(const std::string& typeName) = 0;
    virtual std::shared_ptr<IType> getTypeFor(const std::string& symbol) = 0;
    virtual std::shared_ptr<ITypeContext> createChildTypeContext() = 0;
    virtual bool defineSymbol(const std::string& symbol, std::shared_ptr<IType> type) = 0;
    virtual std::shared_ptr<IType> createType(const std::string& name) = 0;
    
    // Gestión de tipos predefinidos
    virtual bool defineBuiltinType(const std::string& typeName) = 0;
    virtual bool isBuiltinType(const std::string& typeName) const = 0;
};

// Implementación concreta que extiende el Context existente
class TypeContext : public ITypeContext {
private:
    // Delegamos al contexto existente para mantener compatibilidad
    std::shared_ptr<Context> baseContext;
    
    // Nuevas estructuras para el sistema de tipos
    std::unordered_map<std::string, std::shared_ptr<IType>> types;
    std::unordered_map<std::string, std::shared_ptr<IType>> symbolTypes;
    std::shared_ptr<TypeContext> parent;
    
    // Tipos built-in del sistema
    void initializeBuiltinTypes() {
        // Crear tipos primitivos de HULK
        auto numberType = std::make_shared<ConcreteType>("Number");
        auto stringType = std::make_shared<ConcreteType>("String");
        auto boolType = std::make_shared<ConcreteType>("Boolean");
        auto voidType = std::make_shared<ConcreteType>("Void");
        
        // Tipos para iteradores y rangos (específicos de HULK)
        auto rangeType = std::make_shared<ConcreteType>("Range");
        auto iterableType = std::make_shared<ConcreteType>("Iterable");
        
        // Registrar tipos primitivos
        types["Number"] = numberType;
        types["String"] = stringType;
        types["Boolean"] = boolType;
        types["Void"] = voidType;
        types["Range"] = rangeType;
        types["Iterable"] = iterableType;
    }

public:
    TypeContext() : parent(nullptr) {
        baseContext = std::make_shared<Context>();
        initializeBuiltinTypes();
    }
    
    explicit TypeContext(std::shared_ptr<TypeContext> parentContext) 
        : parent(parentContext) {
        baseContext = std::dynamic_pointer_cast<Context>(parentContext->baseContext->createChildContext());
        // Los tipos se heredan del contexto padre
    }
    
    // === Implementación de IContext (delegando al contexto base) ===
    
    bool isDefined(const std::string& name) override {
        return baseContext->isDefined(name);
    }
    
    bool isDefined(const std::string& name, int arity) override {
        return baseContext->isDefined(name, arity);
    }
    
    bool define(const std::string& name) override {
        return baseContext->define(name);
    }
    
    bool define(const std::string& name, const std::vector<std::string>& params) override {
        return baseContext->define(name, params);
    }
    
    std::shared_ptr<IContext> createChildContext() override {
        return std::make_shared<TypeContext>(shared_from_this());
    }
    
    // === Implementación de ITypeContext ===
    
    std::shared_ptr<IType> getType(const std::string& typeName) override {
        // Buscar en el contexto actual
        auto it = types.find(typeName);
        if (it != types.end()) {
            return it->second;
        }
        
        // Buscar en el contexto padre
        if (parent) {
            return parent->getType(typeName);
        }
        
        return nullptr;
    }
    
    std::shared_ptr<IType> getTypeFor(const std::string& symbol) override {
        // Buscar en el contexto actual
        auto it = symbolTypes.find(symbol);
        if (it != symbolTypes.end()) {
            return it->second;
        }
        
        // Buscar en el contexto padre
        if (parent) {
            return parent->getTypeFor(symbol);
        }
        
        return nullptr;
    }
    
    std::shared_ptr<ITypeContext> createChildTypeContext() override {
        return std::make_shared<TypeContext>(std::dynamic_pointer_cast<TypeContext>(shared_from_this()));
    }
    
    bool defineSymbol(const std::string& symbol, std::shared_ptr<IType> type) override {
        // Verificar que el símbolo no esté ya definido en este contexto
        if (symbolTypes.find(symbol) != symbolTypes.end()) {
            return false;
        }
        
        // Definir en el contexto base también
        bool baseResult = baseContext->define(symbol);
        if (!baseResult) {
            return false;
        }
        
        // Asociar el símbolo con su tipo
        symbolTypes[symbol] = type;
        return true;
    }
    
    std::shared_ptr<IType> createType(const std::string& name) override {
        // Verificar que el tipo no exista ya
        if (getType(name) != nullptr) {
            return nullptr; // Tipo ya existe
        }
        
        // Crear nuevo tipo
        auto newType = std::make_shared<ConcreteType>(name);
        types[name] = newType;
        return newType;
    }
    
    bool defineBuiltinType(const std::string& typeName) override {
        if (types.find(typeName) != types.end()) {
            return false; // Ya existe
        }
        
        auto builtinType = std::make_shared<ConcreteType>(typeName);
        types[typeName] = builtinType;
        return true;
    }
    
    bool isBuiltinType(const std::string& typeName) const override {
        static const std::unordered_set<std::string> builtins = {
            "Number", "String", "Boolean", "Void", "Range", "Iterable"
        };
        return builtins.count(typeName) > 0;
    }
    
    // Métodos auxiliares para debugging y testing
    std::vector<std::string> getDefinedTypes() const {
        std::vector<std::string> result;
        for (const auto& pair : types) {
            result.push_back(pair.first);
        }
        return result;
    }
    
    std::vector<std::string> getDefinedSymbols() const {
        std::vector<std::string> result;
        for (const auto& pair : symbolTypes) {
            result.push_back(pair.first + " : " + pair.second->getName());
        }
        return result;
    }
    
private:
    // Para poder usar shared_from_this()
    std::shared_ptr<TypeContext> shared_from_this() {
        return std::static_pointer_cast<TypeContext>(std::enable_shared_from_this<ITypeContext>::shared_from_this());
    }
};

// Implementación que hereda de std::enable_shared_from_this
class TypeContextImpl : public ITypeContext, public std::enable_shared_from_this<TypeContextImpl> {
private:
    std::shared_ptr<Context> baseContext;
    std::unordered_map<std::string, std::shared_ptr<IType>> types;
    std::unordered_map<std::string, std::shared_ptr<IType>> symbolTypes;
    std::shared_ptr<TypeContextImpl> parent;
    
    void initializeBuiltinTypes() {
        auto numberType = std::make_shared<ConcreteType>("Number");
        auto stringType = std::make_shared<ConcreteType>("String");
        auto boolType = std::make_shared<ConcreteType>("Boolean");
        auto voidType = std::make_shared<ConcreteType>("Void");
        auto rangeType = std::make_shared<ConcreteType>("Range");
        auto iterableType = std::make_shared<ConcreteType>("Iterable");
        
        types["Number"] = numberType;
        types["String"] = stringType;
        types["Boolean"] = boolType;
        types["Void"] = voidType;
        types["Range"] = rangeType;
        types["Iterable"] = iterableType;
    }

public:
    TypeContextImpl() : parent(nullptr) {
        baseContext = std::make_shared<Context>();
        initializeBuiltinTypes();
    }
    
    explicit TypeContextImpl(std::shared_ptr<TypeContextImpl> parentContext) 
        : parent(parentContext) {
        baseContext = std::dynamic_pointer_cast<Context>(parentContext->baseContext->createChildContext());
    }
    
    // Implementación de todos los métodos igual que arriba...
    // (Copiamos la implementación completa pero usando shared_from_this() correctamente)
    
    bool isDefined(const std::string& name) override {
        return baseContext->isDefined(name);
    }
    
    bool isDefined(const std::string& name, int arity) override {
        return baseContext->isDefined(name, arity);
    }
    
    bool define(const std::string& name) override {
        return baseContext->define(name);
    }
    
    bool define(const std::string& name, const std::vector<std::string>& params) override {
        return baseContext->define(name, params);
    }
    
    std::shared_ptr<IContext> createChildContext() override {
        return std::make_shared<TypeContextImpl>(shared_from_this());
    }
    
    std::shared_ptr<IType> getType(const std::string& typeName) override {
        auto it = types.find(typeName);
        if (it != types.end()) {
            return it->second;
        }
        
        if (parent) {
            return parent->getType(typeName);
        }
        
        return nullptr;
    }
    
    std::shared_ptr<IType> getTypeFor(const std::string& symbol) override {
        auto it = symbolTypes.find(symbol);
        if (it != symbolTypes.end()) {
            return it->second;
        }
        
        if (parent) {
            return parent->getTypeFor(symbol);
        }
        
        return nullptr;
    }
    
    std::shared_ptr<ITypeContext> createChildTypeContext() override {
        return std::make_shared<TypeContextImpl>(shared_from_this());
    }
    
    bool defineSymbol(const std::string& symbol, std::shared_ptr<IType> type) override {
        if (symbolTypes.find(symbol) != symbolTypes.end()) {
            return false;
        }
        
        bool baseResult = baseContext->define(symbol);
        if (!baseResult) {
            return false;
        }
        
        symbolTypes[symbol] = type;
        return true;
    }
    
    std::shared_ptr<IType> createType(const std::string& name) override {
        if (getType(name) != nullptr) {
            return nullptr;
        }
        
        auto newType = std::make_shared<ConcreteType>(name);
        types[name] = newType;
        return newType;
    }
    
    bool defineBuiltinType(const std::string& typeName) override {
        if (types.find(typeName) != types.end()) {
            return false;
        }
        
        auto builtinType = std::make_shared<ConcreteType>(typeName);
        types[typeName] = builtinType;
        return true;
    }
    
    bool isBuiltinType(const std::string& typeName) const override {
        static const std::unordered_set<std::string> builtins = {
            "Number", "String", "Boolean", "Void", "Range", "Iterable"
        };
        return builtins.count(typeName) > 0;
    }
};

#endif // TYPE_CONTEXT_HPP