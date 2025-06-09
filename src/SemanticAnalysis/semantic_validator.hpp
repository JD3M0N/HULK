#ifndef SEMANTIC_VALIDATOR_HPP
#define SEMANTIC_VALIDATOR_HPP

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include "../AST/ast.hpp"
#include "../Errors/error.hpp"
#include "../Errors/error_types/syntax_error.hpp"
#include "context.hpp"

// Error específico para problemas semánticos
class SemanticError : public Error {
public:
    SemanticError(const std::string& msg, const Location& loc = Location())
        : Error(msg, loc, ErrorLevel::ERROR) {}
    
    std::string getErrorName() const override { 
        return "Error Semántico"; 
    }
};

// Validador semántico principal
class SemanticValidator {
private:
    std::shared_ptr<Context> globalContext;
    std::vector<std::shared_ptr<Error>> errors;
    
public:
    SemanticValidator() {
        // Crear el contexto global
        globalContext = std::make_shared<Context>();
        setupBuiltins();
    }
    
    // Validar un programa completo
    bool validateProgram(Program* program) {
        if (!program) {
            addError("Programa no válido", Location());
            return false;
        }
        
        // Limpiar errores previos
        errors.clear();
        
        // Validar el programa usando el contexto global
        bool isValid = program->validate(globalContext);
        
        return isValid && errors.empty();
    }
    
    // Obtener la lista de errores encontrados
    const std::vector<std::shared_ptr<Error>>& getErrors() const {
        return errors;
    }
    
    // Agregar un error a la lista
    void addError(const std::string& message, const Location& location) {
        errors.push_back(std::make_shared<SemanticError>(message, location));
    }
    
    // Obtener el contexto global (útil para testing)
    std::shared_ptr<IContext> getGlobalContext() const {
        return globalContext;
    }
    
private:
    // Configurar funciones built-in del lenguaje
    void setupBuiltins() {
        // Funciones matemáticas (unarias)
        globalContext->define("sqrt", {"x"});
        globalContext->define("sin", {"x"});
        globalContext->define("cos", {"x"});
        globalContext->define("exp", {"x"});
        globalContext->define("log", {"x"});
        globalContext->define("abs", {"x"});
        
        // Función sin argumentos
        globalContext->define("rand", {});
        
        // Función de salida
        globalContext->define("print", {"value"});
        
        // Funciones para rangos e iteradores
        globalContext->define("range", {"min", "max"});
        globalContext->define("next", {"iterator"});
        globalContext->define("current", {"iterator"});
        globalContext->define("iter", {"range"});
        
        // Funciones de conversión
        globalContext->define("toString", {"value"});
        globalContext->define("toNumber", {"value"});
        
        // Operadores especiales que pueden actuar como funciones
        globalContext->define("pow", {"base", "exponent"});
        globalContext->define("mod", {"dividend", "divisor"});
    }
};

// Versión extendida del validador que integra con el sistema de errores existente
class IntegratedSemanticValidator : public SemanticValidator {
public:
    // Validar y reportar errores usando el sistema de manejo de errores existente
    bool validateAndReport(Program* program) {
        bool isValid = validateProgram(program);
        
        return isValid;
    }
    
    // Validar un nodo específico con contexto personalizado
    bool validateNode(Expr* expr, std::shared_ptr<IContext> context = nullptr) {
        if (!expr) return false;
        
        auto contextToUse = context ? context : getGlobalContext();
        return expr->validate(contextToUse);
    }
    
    // Validar una función específica antes de su definición
    bool validateFunctionSignature(const std::string& name, 
                                 const std::vector<std::string>& params,
                                 const Location& location) {
        // Verificar que no haya parámetros duplicados
        std::unordered_set<std::string> paramSet;
        for (const auto& param : params) {
            if (paramSet.count(param)) {
                addError("Parámetro duplicado: " + param, location);
                return false;
            }
            paramSet.insert(param);
        }
        
        // Verificar que el nombre no sea una palabra reservada o función built-in
        if (isBuiltinFunction(name)) {
            addError("No se puede redefinir la función built-in: " + name, location);
            return false;
        }
        
        return true;
    }
    
private:
    bool isBuiltinFunction(const std::string& name) const {
        // Lista de funciones que no pueden ser redefinidas
        static const std::unordered_set<std::string> builtins = {
            "sqrt", "sin", "cos", "exp", "log", "abs", "rand", "print",
            "range", "next", "current", "iter", "toString", "toNumber",
            "pow", "mod"
        };
        return builtins.count(name) > 0;
    }
};

#endif // SEMANTIC_VALIDATOR_HPP