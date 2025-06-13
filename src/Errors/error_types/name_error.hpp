#ifndef NAME_ERROR_HPP
#define NAME_ERROR_HPP

#include "../error.hpp"

class NameError : public Error {
public:
    NameError(const std::string& msg, const Location& loc = Location())
        : Error(msg, loc, ErrorLevel::ERROR) {}
    
    std::string getErrorName() const override { 
        return "Error de Nombre"; 
    }
};

#endif // NAME_ERROR_HPP