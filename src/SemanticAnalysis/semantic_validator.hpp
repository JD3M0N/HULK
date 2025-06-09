#ifndef SEMANTIC_VALIDATOR_HPP
#define SEMANTIC_VALIDATOR_HPP

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
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
        return "Error Semantico"; 
    }
};

// Validador semántico principal con logging detallado
class SemanticValidator {
private:
    std::shared_ptr<Context> globalContext;
    std::vector<std::shared_ptr<Error>> errors;
    bool verboseMode;
    int indentLevel;
    
public:
    SemanticValidator(bool verbose = true) : verboseMode(verbose), indentLevel(0) {
        // Crear el contexto global
        globalContext = std::make_shared<Context>();
        setupBuiltins();
    }
    
    // Validar un programa completo
    bool validateProgram(Program* program) {
        if (!program) {
            addError("Programa no valido", Location());
            return false;
        }
        
        // Limpiar errores previos
        errors.clear();
        
        if (verboseMode) {
            std::cout << "[SEMANTIC] Iniciando validacion semantica...\n";
            std::cout << "[BUILTIN ] Contexto global configurado con funciones built-in\n";
            printBuiltins();
            std::cout << "\n";
        }
        
        // Validar el programa usando el contexto global Y pasando la referencia del validador
        bool isValid = program->validate(globalContext, this);
        
        if (verboseMode) {
            std::cout << "\n[RESUMEN ] Resumen de validacion:\n";
            if (!errors.empty()) {
                std::cout << "[ERROR  ] " << errors.size() << " error(es) encontrado(s)\n";
            } else {
                std::cout << "[SUCCESS] Sin errores semanticos\n";
            }
        }
        
        return isValid && errors.empty();
    }
    
    // Obtener la lista de errores encontrados
    const std::vector<std::shared_ptr<Error>>& getErrors() const {
        return errors;
    }
    
    // Agregar un error a la lista
    void addError(const std::string& message, const Location& location) {
        if (verboseMode) {
            std::cout << "[ERROR  ] " << message;
            if (location.line > 0) {
                std::cout << " (linea " << location.line << ")";
            }
            std::cout << "\n";
        }
        errors.push_back(std::make_shared<SemanticError>(message, location));
    }
    
    // Obtener el contexto global (útil para testing)
    std::shared_ptr<IContext> getGlobalContext() const {
        return globalContext;
    }
    
    // Logging para validación de variables
    void logVariableCheck(const std::string& varName, bool isDefined, const std::string& context = "") {
        if (!verboseMode) return;
        
        printIndent();
        if (isDefined) {
            std::cout << "[VAR-OK ] Variable '" << varName << "' encontrada";
        } else {
            std::cout << "[VAR-ERR] Variable '" << varName << "' NO definida";
        }
        if (!context.empty()) {
            std::cout << " en " << context;
        }
        std::cout << "\n";
    }
    
    // Logging para validación de funciones
    void logFunctionCheck(const std::string& funcName, int arity, bool isDefined, const std::string& context = "") {
        if (!verboseMode) return;
        
        printIndent();
        if (isDefined) {
            std::cout << "[FUNC-OK] Funcion '" << funcName << "(" << arity << " args)' encontrada";
        } else {
            std::cout << "[FUNC-ER] Funcion '" << funcName << "(" << arity << " args)' NO definida";
        }
        if (!context.empty()) {
            std::cout << " en " << context;
        }
        std::cout << "\n";
    }
    
    // Logging para definición de variables
    void logVariableDefinition(const std::string& varName, bool success, const std::string& context = "") {
        if (!verboseMode) return;
        
        printIndent();
        if (success) {
            std::cout << "[VAR-DEF] Variable '" << varName << "' definida";
        } else {
            std::cout << "[VAR-DUP] Variable '" << varName << "' ya existe";
        }
        if (!context.empty()) {
            std::cout << " en " << context;
        }
        std::cout << "\n";
    }
    
    // Logging para definición de funciones
    void logFunctionDefinition(const std::string& funcName, const std::vector<std::string>& params, bool success) {
        if (!verboseMode) return;
        
        printIndent();
        if (success) {
            std::cout << "[FUN-DEF] Funcion '" << funcName << "(";
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << params[i];
            }
            std::cout << ")' definida\n";
        } else {
            std::cout << "[FUN-DUP] Funcion '" << funcName << "' con " << params.size() << " parametros ya existe\n";
        }
    }
    
    // Logging para entrada/salida de contextos
    void logContextEnter(const std::string& contextType) {
        if (!verboseMode) return;
        
        printIndent();
        std::cout << "[CONTEXT] Entrando a " << contextType << "\n";
        indentLevel++;
    }
    
    void logContextExit(const std::string& contextType) {
        if (!verboseMode) return;
        
        indentLevel--;
        printIndent();
        std::cout << "[CONTEXT] Saliendo de " << contextType << "\n";
    }
    
    // Activar/desactivar modo verbose
    void setVerbose(bool verbose) {
        verboseMode = verbose;
    }
    
private:
    void printIndent() {
        for (int i = 0; i < indentLevel; ++i) {
            std::cout << "  ";
        }
    }
    
    void printBuiltins() {
        if (!verboseMode) return;
        
        std::cout << "[BUILTIN] Funciones built-in disponibles:\n";
        std::cout << "[BUILTIN]   - Matematicas: sqrt(x), sin(x), cos(x), exp(x), log(x), abs(x)\n";
        std::cout << "[BUILTIN]   - Sin parametros: rand()\n";
        std::cout << "[BUILTIN]   - E/S: print(value)\n";
        std::cout << "[BUILTIN]   - Iteradores: range(min,max), next(iter), current(iter), iter(range)\n";
        std::cout << "[BUILTIN]   - Conversion: toString(value), toNumber(value)\n";
        std::cout << "[BUILTIN]   - Operadores: pow(base,exp), mod(dividend,divisor)\n";
    }
    
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
    IntegratedSemanticValidator(bool verbose = true) : SemanticValidator(verbose) {}
    
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
                addError("Parametro duplicado: " + param, location);
                return false;
            }
            paramSet.insert(param);
        }
        
        // Verificar que el nombre no sea una palabra reservada o función built-in
        if (isBuiltinFunction(name)) {
            addError("No se puede redefinir la funcion built-in: " + name, location);
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