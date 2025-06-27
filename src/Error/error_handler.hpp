#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

enum class ErrorType {
    LEXICAL_ERROR,
    SYNTAX_ERROR,
    SEMANTIC_ERROR,
    TYPE_ERROR,
    SCOPE_ERROR,
    CODEGEN_ERROR,
    RUNTIME_ERROR,
    INTERNAL_ERROR
};

enum class ErrorSeverity {
    WARNING,
    ERROR,
    FATAL
};

struct SourceLocation {
    std::string filename;
    int line;
    int column;
    
    SourceLocation(const std::string& file = "", int ln = 0, int col = 0)
        : filename(file), line(ln), column(col) {}
        
    std::string toString() const {
        std::stringstream ss;
        if (!filename.empty()) {
            ss << filename;
        }
        if (line > 0) {
            ss << ":" << line;
            if (column > 0) {
                ss << ":" << column;
            }
        }
        return ss.str();
    }
};

class ErrorMessage {
private:
    ErrorType type;
    ErrorSeverity severity;
    std::string message;
    SourceLocation location;
    std::string context;  // Línea de código donde ocurrió el error
    
public:
    ErrorMessage(ErrorType t, ErrorSeverity s, const std::string& msg, 
                 const SourceLocation& loc = SourceLocation(), 
                 const std::string& ctx = "")
        : type(t), severity(s), message(msg), location(loc), context(ctx) {}
    
    // Getters
    ErrorType getType() const { return type; }
    ErrorSeverity getSeverity() const { return severity; }
    const std::string& getMessage() const { return message; }
    const SourceLocation& getLocation() const { return location; }
    const std::string& getContext() const { return context; }
    
    std::string toString() const;
    std::string getTypeString() const;
    std::string getSeverityString() const;
};

class ErrorHandler {
private:
    std::vector<ErrorMessage> errors;
    int errorCount;
    int warningCount;
    int maxErrors;
    bool shouldAbort;
    
public:
    ErrorHandler(int maxErr = 10) : errorCount(0), warningCount(0), 
                                   maxErrors(maxErr), shouldAbort(false) {}
    
    // Métodos para reportar errores
    void reportError(ErrorType type, const std::string& message, 
                    const SourceLocation& location = SourceLocation(),
                    const std::string& context = "");
                    
    void reportWarning(ErrorType type, const std::string& message,
                      const SourceLocation& location = SourceLocation(),
                      const std::string& context = "");
                      
    void reportFatal(ErrorType type, const std::string& message,
                    const SourceLocation& location = SourceLocation(),
                    const std::string& context = "");
    
    // Métodos de consulta
    bool hasErrors() const { return errorCount > 0; }
    bool hasWarnings() const { return warningCount > 0; }
    bool shouldAbortCompilation() const { return shouldAbort || errorCount >= maxErrors; }
    int getErrorCount() const { return errorCount; }
    int getWarningCount() const { return warningCount; }
    
    // Métodos de salida
    void printErrors(std::ostream& out = std::cerr) const;
    void printSummary(std::ostream& out = std::cerr) const;
    std::string generateReport() const;
    
    // Configuración
    void setMaxErrors(int max) { maxErrors = max; }
    void clear();
    
    // Métodos de conveniencia para diferentes tipos de errores
    void lexicalError(const std::string& message, const SourceLocation& loc = SourceLocation(), const std::string& ctx = "");
    void syntaxError(const std::string& message, const SourceLocation& loc = SourceLocation(), const std::string& ctx = "");
    void semanticError(const std::string& message, const SourceLocation& loc = SourceLocation(), const std::string& ctx = "");
    void typeError(const std::string& message, const SourceLocation& loc = SourceLocation(), const std::string& ctx = "");
    void scopeError(const std::string& message, const SourceLocation& loc = SourceLocation(), const std::string& ctx = "");
    void codegenError(const std::string& message, const SourceLocation& loc = SourceLocation(), const std::string& ctx = "");
    void runtimeError(const std::string& message, const SourceLocation& loc = SourceLocation(), const std::string& ctx = "");
    void internalError(const std::string& message, const SourceLocation& loc = SourceLocation(), const std::string& ctx = "");
    
    // Singleton pattern
    static ErrorHandler& getInstance();
};

// Macros para facilitar el reporte de errores
#define REPORT_ERROR(type, msg, loc, ctx) ErrorHandler::getInstance().reportError(type, msg, loc, ctx)
#define REPORT_WARNING(type, msg, loc, ctx) ErrorHandler::getInstance().reportWarning(type, msg, loc, ctx)
#define REPORT_FATAL(type, msg, loc, ctx) ErrorHandler::getInstance().reportFatal(type, msg, loc, ctx)

// Macros específicas por tipo
#define LEXICAL_ERROR(msg, loc, ctx) ErrorHandler::getInstance().lexicalError(msg, loc, ctx)
#define SYNTAX_ERROR(msg, loc, ctx) ErrorHandler::getInstance().syntaxError(msg, loc, ctx)
#define SEMANTIC_ERROR(msg, loc, ctx) ErrorHandler::getInstance().semanticError(msg, loc, ctx)
#define TYPE_ERROR(msg, loc, ctx) ErrorHandler::getInstance().typeError(msg, loc, ctx)
#define SCOPE_ERROR(msg, loc, ctx) ErrorHandler::getInstance().scopeError(msg, loc, ctx)
#define CODEGEN_ERROR(msg, loc, ctx) ErrorHandler::getInstance().codegenError(msg, loc, ctx)
#define RUNTIME_ERROR(msg, loc, ctx) ErrorHandler::getInstance().runtimeError(msg, loc, ctx)
#define INTERNAL_ERROR(msg, loc, ctx) ErrorHandler::getInstance().internalError(msg, loc, ctx)

#endif // ERROR_HANDLER_HPP
