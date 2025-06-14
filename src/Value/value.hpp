// value.hpp
#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include "../Scope/scope.hpp"

// Forward declarations
struct RangeValue;
struct IterableValue;
struct ObjectValue;
struct ClassDecl;

class Value
{
public:
    enum Type
    {
        NUMBER,
        STRING,
        BOOLEAN,
        OBJECT,
        RANGE,
        ITERABLE
    };

private:
    Type type_;
    double number_;
    std::string string_;
    bool boolean_;
    std::shared_ptr<ObjectValue> object_;
    std::shared_ptr<RangeValue> range_;
    std::shared_ptr<IterableValue> iterable_;

public:
    // Constructores
    Value() : type_(NUMBER), number_(0.0) {}
    Value(double n) : type_(NUMBER), number_(n) {}
    Value(const std::string &s) : type_(STRING), string_(s) {}
    Value(bool b) : type_(BOOLEAN), boolean_(b) {}
    Value(std::shared_ptr<ObjectValue> obj) : type_(OBJECT), object_(obj) {}
    Value(std::shared_ptr<RangeValue> r) : type_(RANGE), range_(r) {}
    Value(std::shared_ptr<IterableValue> it) : type_(ITERABLE), iterable_(it) {}

    // Getters de tipo
    bool isNumber() const { return type_ == NUMBER; }
    bool isString() const { return type_ == STRING; }
    bool isBool() const { return type_ == BOOLEAN; }
    bool isObject() const { return type_ == OBJECT; }
    bool isRange() const { return type_ == RANGE; }
    bool isIterable() const { return type_ == ITERABLE; }

    // Conversiones
    double asNumber() const { return number_; }
    const std::string &asString() const { return string_; }
    bool asBool() const { return boolean_; }
    std::shared_ptr<ObjectValue> asObject() const { return object_; }
    std::shared_ptr<RangeValue> asRange() const { return range_; }
    std::shared_ptr<IterableValue> asIterable() const { return iterable_; }
};

// Estructura para objetos - ahora usa Scope en lugar de EnvFrame
struct ObjectValue
{
    std::string className;
    Scope<Value>::Ptr fields; // Cambiado de EnvFrame a Scope<Value>
    ClassDecl *classDef;

    ObjectValue(const std::string &name, ClassDecl *def) 
        : className(name), classDef(def)
    {
        // Crear un scope para los campos del objeto
        fields = std::make_shared<Scope<Value>>(nullptr);
    }
};