#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include "location.hpp"

enum class ErrorLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Error {
protected:
    std::string message;
    Location location;
    ErrorLevel level;
    
public:
    Error(const std::string& msg, const Location& loc = Location(), 
          ErrorLevel lvl = ErrorLevel::ERROR)
        : message(msg), location(loc), level(lvl) {}
    
    virtual ~Error() = default;
    
    virtual std::string getErrorName() const { 
        return "Error"; 
    }
    
    const std::string& getMessage() const { 
        return message; 
    }
    
    const Location& getLocation() const { 
        return location; 
    }
    
    ErrorLevel getLevel() const { 
        return level; 
    }
    
    std::string getFullMessage() const {
        std::string levelStr;
        switch (level) {
            case ErrorLevel::INFO:    levelStr = "Info"; break;
            case ErrorLevel::WARNING: levelStr = "Advertencia"; break;
            case ErrorLevel::ERROR:   levelStr = "Error"; break;
            case ErrorLevel::FATAL:   levelStr = "Error Fatal"; break;
        }
        
        return levelStr + " [" + getErrorName() + "]: " + message + 
               (location.line > 0 ? " en " + location.toString() : "");
    }
};

#endif // ERROR_HPP