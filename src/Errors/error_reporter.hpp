#ifndef ERROR_REPORTER_HPP
#define ERROR_REPORTER_HPP

#include <iostream>
#include <string>
#include <vector>
#include "error.hpp"

class ErrorReporter {
private:
    std::ostream& output;
    bool useColors;
    
    std::string getColorCode(ErrorLevel level) const {
        if (!useColors) return "";
        
        switch (level) {
            case ErrorLevel::INFO:    return "\033[36m"; // Cian
            case ErrorLevel::WARNING: return "\033[33m"; // Amarillo
            case ErrorLevel::ERROR:   return "\033[31m"; // Rojo
            case ErrorLevel::FATAL:   return "\033[1;31m"; // Rojo brillante
            default:                  return "\033[0m"; // Resetear
        }
    }
    
    std::string resetColor() const {
        return useColors ? "\033[0m" : "";
    }
    
public:
    ErrorReporter(std::ostream& os = std::cerr, bool colors = true) 
        : output(os), useColors(colors) {}
    
    void report(const Error& error) {
        output << getColorCode(error.getLevel()) 
               << error.getFullMessage() 
               << resetColor() << std::endl;
    }
    
    void reportAll(const std::vector<std::shared_ptr<Error>>& errors) {
        for (const auto& error : errors) {
            report(*error);
        }
    }
    
    void setUseColors(bool use) {
        useColors = use;
    }
};

#endif // ERROR_REPORTER_HPP