#ifndef SEMANTIC_VALIDATOR_HPP
#define SEMANTIC_VALIDATOR_HPP

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include "../AST/ast.hpp"
#include "../Errors/error.hpp"
#include "../Errors/error_types/syntax_error.hpp"
#include "../Errors/error_types/semantic_error.hpp"
#include "context.hpp"

// Validador semántico principal con logging detallado
class SemanticValidator {
private:
    std::shared_ptr<Context> globalContext;
    std::vector<std::shared_ptr<Error>> errors;
    bool verboseMode;
    int indentLevel;
    std::vector<std::string> contextStack; // Pila de contextos para mostrar jerarquía
    
public:
    SemanticValidator(bool verbose = true) : verboseMode(verbose), indentLevel(0) {
        // Crear el contexto global
        globalContext = std::make_shared<Context>();
        setupBuiltins();
        contextStack.push_back("Global");
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
            printHeader("VALIDACION SEMANTICA");
            printSeparator();
            std::cout << "Iniciando validacion del programa...\n";
            printBuiltins();
            printSeparator();
        }
        
        // Validar el programa usando el contexto global Y pasando la referencia del validador
        bool isValid = program->validate(globalContext, this);
        
        if (verboseMode) {
            printSeparator();
            printSummary();
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
            printIndent();
            std::cout << "[ERROR] " << message;
            if (location.line > 0) {
                std::cout << " (linea " << location.line;
                if (location.column > 0) {
                    std::cout << ", col " << location.column;
                }
                std::cout << ")";
            }
            std::cout << " | Contexto: " << getCurrentContextPath();
            std::cout << "\n";
        }
        
        auto error = std::make_shared<SemanticError>(message, location);
        errors.push_back(error);
        
        // Debug: Mostrar que el error fue añadido
        if (verboseMode) {
            printIndent();
            std::cout << "    [DEBUG] Error agregado. Total errores: " << errors.size() << "\n";
        }
    }
    
    // Obtener el contexto global (útil para testing)
    std::shared_ptr<IContext> getGlobalContext() const {
        return globalContext;
    }
    
    // Logging para validación de variables
    void logVariableCheck(const std::string& varName, bool isDefined, const std::string& context = "") {
        if (!verboseMode) return;
        
        printIndent();
        std::cout << (isDefined ? "[VAR-OK]" : "[VAR-ER]");
        std::cout << " Variable '" << varName << "'";
        std::cout << (isDefined ? " encontrada" : " NO definida");
        
        if (!context.empty()) {
            std::cout << " en " << context;
        }
        
        std::cout << " | Contexto actual: " << getCurrentContextPath();
        std::cout << "\n";
        
        if (!isDefined) {
            printAvailableVariables();
        }
    }
    
    // Logging para validación de funciones
    void logFunctionCheck(const std::string& funcName, int arity, bool isDefined, const std::string& context = "") {
        if (!verboseMode) return;
        
        printIndent();
        std::cout << (isDefined ? "[FUNC-OK]" : "[FUNC-ER]");
        std::cout << " Funcion '" << funcName << "(" << arity << " args)'";
        std::cout << (isDefined ? " encontrada" : " NO definida");
        
        if (!context.empty()) {
            std::cout << " en " << context;
        }
        
        std::cout << " | Contexto: " << getCurrentContextPath();
        std::cout << "\n";
        
        if (!isDefined) {
            printAvailableFunctions(arity);
        }
    }
    
    // Logging para definición de variables
    void logVariableDefinition(const std::string& varName, bool success, const std::string& context = "") {
        if (!verboseMode) return;
        
        printIndent();
        std::cout << (success ? "[VAR-DEF]" : "[VAR-DUP]");
        std::cout << " Variable '" << varName << "'";
        std::cout << (success ? " definida" : " ya existe");
        
        if (!context.empty()) {
            std::cout << " en " << context;
        }
        
        std::cout << " | Contexto: " << getCurrentContextPath();
        std::cout << "\n";
    }
    
    // Logging para definición de funciones
    void logFunctionDefinition(const std::string& funcName, const std::vector<std::string>& params, bool success) {
        if (!verboseMode) return;
        
        printIndent();
        std::cout << (success ? "[FUN-DEF]" : "[FUN-DUP]");
        std::cout << " Funcion '" << funcName << "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << params[i];
        }
        std::cout << ")'";
        std::cout << (success ? " definida" : " ya existe");
        std::cout << " | Contexto: " << getCurrentContextPath();
        std::cout << "\n";
        
        if (success && !params.empty()) {
            printIndent();
            std::cout << "    Parametros: ";
            for (size_t i = 0; i < params.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << params[i];
            }
            std::cout << "\n";
        }
    }
    
    // Logging para entrada/salida de contextos
    void logContextEnter(const std::string& contextType) {
        if (!verboseMode) return;
        
        printIndent();
        std::cout << "[ENTER] " << contextType;
        std::cout << " | Ruta: " << getCurrentContextPath() << " -> " << contextType;
        std::cout << "\n";
        
        contextStack.push_back(contextType);
        indentLevel++;
    }
    
    void logContextExit(const std::string& contextType) {
        if (!verboseMode) return;
        
        if (!contextStack.empty() && contextStack.back() == contextType) {
            contextStack.pop_back();
        }
        
        indentLevel--;
        printIndent();
        std::cout << "[EXIT ] " << contextType;
        std::cout << " | Volviendo a: " << getCurrentContextPath();
        std::cout << "\n";
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
    
    void printHeader(const std::string& title) {
        std::cout << "\n";
        printSeparator();
        std::cout << "  " << title << "\n";
        printSeparator();
    }
    
    void printSeparator() {
        std::cout << std::string(60, '=') << "\n";
    }
    
    void printSubSeparator() {
        std::cout << std::string(40, '-') << "\n";
    }
    
    std::string getCurrentContextPath() {
        if (contextStack.empty()) return "Global";
        
        std::string path = "";
        for (size_t i = 0; i < contextStack.size(); ++i) {
            if (i > 0) path += " > ";
            path += contextStack[i];
        }
        return path;
    }
    
    void printAvailableVariables() {
        // Esta función necesitaría acceso al contexto actual para mostrar variables disponibles
        // Por ahora, solo mostramos un mensaje indicativo
        printIndent();
        std::cout << "    Sugerencia: Verificar variables disponibles en contexto actual\n";
    }
    
    void printAvailableFunctions(int expectedArity) {
        printIndent();
        std::cout << "    Funciones disponibles con " << expectedArity << " parametros:\n";
        
        // Mostrar algunas funciones built-in relevantes
        if (expectedArity == 1) {
            printIndent();
            std::cout << "      sqrt, sin, cos, exp, log, abs, print, toString, toNumber\n";
        } else if (expectedArity == 2) {
            printIndent();
            std::cout << "      range, pow, mod\n";
        } else if (expectedArity == 0) {
            printIndent();
            std::cout << "      rand\n";
        }
    }
    
    void printBuiltins() {
        if (!verboseMode) return;
        
        std::cout << "[SETUP] Configurando funciones built-in:\n";
        printSubSeparator();
        std::cout << "  Matematicas (1 arg): sqrt, sin, cos, exp, log, abs\n";
        std::cout << "  Sin parametros:      rand\n";
        std::cout << "  Entrada/Salida:      print\n";
        std::cout << "  Iteradores:          range(min,max), next(iter), current(iter), iter(range)\n";
        std::cout << "  Conversion:          toString, toNumber\n";
        std::cout << "  Operadores:          pow(base,exp), mod(dividend,divisor)\n";
    }
    
    void printSummary() {
        std::cout << "RESUMEN DE VALIDACION:\n";
        printSubSeparator();
        std::cout << "Total de errores encontrados: " << errors.size() << "\n";
        
        if (errors.empty()) {
            std::cout << "[SUCCESS] Validacion semantica exitosa - Sin errores encontrados\n";
        } else {
            std::cout << "[FAILED ] " << errors.size() << " error(es) semantico(s) encontrado(s):\n\n";
            
            for (size_t i = 0; i < errors.size(); ++i) {
                std::cout << "  " << (i + 1) << ". ";
                
                // Usar getMessage() directamente ya que getErrorName() puede no existir
                std::cout << errors[i]->getMessage();
                
                if (errors[i]->getLocation().line > 0) {
                    std::cout << " (linea " << errors[i]->getLocation().line;
                    if (errors[i]->getLocation().column > 0) {
                        std::cout << ", col " << errors[i]->getLocation().column;
                    }
                    std::cout << ")";
                }
                std::cout << "\n";
            }
        }
        
        printSeparator();
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