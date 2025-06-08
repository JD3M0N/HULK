#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>

struct Location {
    int line;
    int column;
    std::string filename;
    
    Location() : line(0), column(0), filename("") {}
    
    Location(int l, int c, const std::string& file = "")
        : line(l), column(c), filename(file) {}
    
    std::string toString() const {
        if (filename.empty()) {
            return "línea " + std::to_string(line) + ", columna " + std::to_string(column);
        } else {
            return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
        }
    }
};

#endif // LOCATION_HPP