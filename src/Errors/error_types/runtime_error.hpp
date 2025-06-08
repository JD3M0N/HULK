#ifndef RUNTIME_ERROR_HPP
#define RUNTIME_ERROR_HPP

#include "../error.hpp"

class RuntimeError : public Error {
public:
    RuntimeError(const std::string& msg, const Location& loc = Location())
        : Error(msg, loc, ErrorLevel::ERROR) {}
    
    std::string getErrorName() const override { 
        return "Error en Tiempo de Ejecución"; 
    }
};

#endif // RUNTIME_ERROR_HPP