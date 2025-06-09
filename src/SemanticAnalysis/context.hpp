#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Interfaz para el contexto semántico que maneja ámbitos (scopes)
class IContext {
public:
    virtual ~IContext() = default;
    
    // Verificar si una variable está definida
    virtual bool isDefined(const std::string& variable) const = 0;
    
    // Verificar si una función está definida con cierta cantidad de argumentos
    virtual bool isDefined(const std::string& function, int args) const = 0;
    
    // Definir una variable en el contexto actual
    virtual bool define(const std::string& variable) = 0;
    
    // Definir una función con sus argumentos en el contexto actual
    virtual bool define(const std::string& function, const std::vector<std::string>& args) = 0;
    
    // Crear un contexto hijo que hereda del actual
    virtual std::shared_ptr<IContext> createChildContext() = 0;
};

// Implementación concreta del contexto semántico con soporte para shared_from_this
class Context : public IContext, public std::enable_shared_from_this<Context> {
private:
    // Contexto padre (nullptr si es el contexto global)
    std::shared_ptr<IContext> parent;
    
    // Variables definidas en este contexto
    std::unordered_set<std::string> variables;
    
    // Funciones definidas en este contexto con sus argumentos
    // La clave es el nombre de la función, el valor es el vector de nombres de argumentos
    std::unordered_map<std::string, std::vector<std::string>> functions;

public:
    // Constructor: recibe el contexto padre
    explicit Context(std::shared_ptr<IContext> p = nullptr) : parent(std::move(p)) {}
    
    // Verificar si una variable está definida (busca recursivamente en contextos padre)
    bool isDefined(const std::string& variable) const override {
        if (variables.count(variable)) {
            return true;
        }
        return parent != nullptr && parent->isDefined(variable);
    }
    
    // Verificar si una función está definida con cierta cantidad de argumentos
    bool isDefined(const std::string& function, int args) const override {
        auto it = functions.find(function);
        if (it != functions.end() && static_cast<int>(it->second.size()) == args) {
            return true;
        }
        return parent != nullptr && parent->isDefined(function, args);
    }
    
    // Definir una variable en el contexto actual
    // Retorna true si se pudo definir, false si ya existía en este contexto
    bool define(const std::string& variable) override {
        if (variables.count(variable)) {
            return false; // Ya está definida en este contexto
        }
        variables.insert(variable);
        return true;
    }
    
    // Definir una función con sus argumentos en el contexto actual
    // Retorna true si se pudo definir, false si ya existía una función con el mismo nombre y aridad
    bool define(const std::string& function, const std::vector<std::string>& args) override {
        auto it = functions.find(function);
        if (it != functions.end() && it->second.size() == args.size()) {
            return false; // Ya está definida con la misma aridad
        }
        functions[function] = args;
        return true;
    }
    
    // Crear un contexto hijo que hereda del actual
    std::shared_ptr<IContext> createChildContext() override {
        return std::make_shared<Context>(shared_from_this());
    }
};

#endif // CONTEXT_HPP