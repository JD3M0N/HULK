#ifndef TYPE_ERROR_HPP
#define TYPE_ERROR_HPP

#include "../error.hpp"

class TypeError : public Error {
public:
    TypeError(const std::string& msg, const Location& loc = Location())
        : Error(msg, loc, ErrorLevel::ERROR) {}
    
    std::string getErrorName() const override { 
        return "Error de Tipo"; 
    }
};

#endif // TYPE_ERROR_HPP