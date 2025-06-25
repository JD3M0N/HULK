// value.hpp
#pragma once

#ifndef VALUE_HPP
#define VALUE_HPP

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

class RangeValue;
class RangeIterator;

struct EnvFrame;
struct ClassDecl;

struct ObjectValue;
using ObjectPtr = std::shared_ptr<ObjectValue>;

struct ObjectValue
{
    // tabla de atributos (runtime)
    std::shared_ptr<EnvFrame> fields;
    // puntero a la ClassDecl estática
    ClassDecl* klass;
    explicit ObjectValue(ClassDecl* k,
                         std::shared_ptr<EnvFrame> f)
        : fields(std::move(f)), klass(k) {}
};

class Value
{
public:
    // Tipo para identificar qué valor está activo en la unión
    enum class ValueType {
        NUMBER,
        STRING,
        BOOLEAN,
        RANGE,
        ITERATOR,
        OBJECT
    };

    Value() : type(ValueType::NUMBER) { 
        new (&data.number) double(0.0);
    }
    
    Value(double d) : type(ValueType::NUMBER) {
        new (&data.number) double(d);
    }
    
    Value(const std::string &s) : type(ValueType::STRING) {
        new (&data.string_buffer) std::string(s);
    }
    
    Value(bool b) : type(ValueType::BOOLEAN) {
        new (&data.boolean) bool(b);
    }
    
    Value(std::shared_ptr<RangeValue> rv) : type(ValueType::RANGE) {
        new (&data.range_buffer) std::shared_ptr<RangeValue>(rv);
    }
    
    Value(std::shared_ptr<RangeIterator> it) : type(ValueType::ITERATOR) {
        new (&data.iterator_buffer) std::shared_ptr<RangeIterator>(it);
    }
    
    Value(ObjectPtr obj) : type(ValueType::OBJECT) {
        new (&data.object_buffer) ObjectPtr(std::move(obj));
    }

    // Constructor de copia
    Value(const Value& other) : type(other.type) {
        switch (type) {
            case ValueType::NUMBER:
                new (&data.number) double(other.data.number);
                break;
            case ValueType::STRING:
                new (&data.string_buffer) std::string(*reinterpret_cast<const std::string*>(&other.data.string_buffer));
                break;
            case ValueType::BOOLEAN:
                new (&data.boolean) bool(other.data.boolean);
                break;
            case ValueType::RANGE:
                new (&data.range_buffer) std::shared_ptr<RangeValue>(*reinterpret_cast<const std::shared_ptr<RangeValue>*>(&other.data.range_buffer));
                break;
            case ValueType::ITERATOR:
                new (&data.iterator_buffer) std::shared_ptr<RangeIterator>(*reinterpret_cast<const std::shared_ptr<RangeIterator>*>(&other.data.iterator_buffer));
                break;
            case ValueType::OBJECT:
                new (&data.object_buffer) ObjectPtr(*reinterpret_cast<const ObjectPtr*>(&other.data.object_buffer));
                break;
        }
    }

    // Operador de asignación
    Value& operator=(const Value& other) {
        if (this != &other) {
            this->~Value();
            new (this) Value(other);
        }
        return *this;
    }

    // Destructor
    ~Value() {
        switch (type) {
            case ValueType::STRING:
                reinterpret_cast<std::string*>(&data.string_buffer)->~basic_string();
                break;
            case ValueType::RANGE:
                reinterpret_cast<std::shared_ptr<RangeValue>*>(&data.range_buffer)->~shared_ptr();
                break;
            case ValueType::ITERATOR:
                reinterpret_cast<std::shared_ptr<RangeIterator>*>(&data.iterator_buffer)->~shared_ptr();
                break;
            case ValueType::OBJECT:
                reinterpret_cast<ObjectPtr*>(&data.object_buffer)->~shared_ptr();
                break;
            default:
                break; // No necesita destrucción manual
        }
    }

    bool isNumber() const { return type == ValueType::NUMBER; }
    bool isString() const { return type == ValueType::STRING; }
    bool isBool() const { return type == ValueType::BOOLEAN; }
    bool isRange() const { return type == ValueType::RANGE; }
    bool isIterable() const { return type == ValueType::ITERATOR; }
    bool isObject() const { return type == ValueType::OBJECT; }

    ObjectPtr asObject() const {
        if (!isObject())
            throw std::runtime_error("Value no es ObjectPtr");
        return *reinterpret_cast<const ObjectPtr*>(&data.object_buffer);
    }

    double asNumber() const {
        if (!isNumber())
            throw std::runtime_error("Value no es número");
        return data.number;
    }

    const std::string& asString() const {
        if (!isString())
            throw std::runtime_error("Value no es string");
        return *reinterpret_cast<const std::string*>(&data.string_buffer);
    }

    bool asBool() const {
        if (!isBool())
            throw std::runtime_error("Value no es boolean");
        return data.boolean;
    }

    std::shared_ptr<RangeValue> asRange() const {
        if (!isRange())
            throw std::runtime_error("Value no es RangeValue");
        return *reinterpret_cast<const std::shared_ptr<RangeValue>*>(&data.range_buffer);
    }

    std::shared_ptr<RangeIterator> asIterable() const {
        if (!isIterable())
            throw std::runtime_error("Value no es RangeIterator");
        return *reinterpret_cast<const std::shared_ptr<RangeIterator>*>(&data.iterator_buffer);
    }

    std::string
    toString() const
    {
        if (isString())
        {
            return asString();
        }
        if (isNumber())
        {
            // Convierte double a string sin ceros inútiles
            std::ostringstream oss;
            oss << asNumber();
            return oss.str();
        }
        if (isBool())
        {
            return asBool() ? "true" : "false";
        }
        if (isRange())
        {
            return "<range>";
        }
        if (isIterable())
        {
            return "<iterator>";
        }
        if (isObject())
        {
            return "<object>";
        }
        return "<unknown>";
    }

    // Método para obtener el nombre del tipo como string
    std::string
    getTypeName() const
    {
        if (isNumber())
            return "número";
        if (isString())
            return "texto";
        if (isBool())
            return "booleano";
        if (isIterable())
            return "iterable";
        if (isRange())
            return "rango";
        return "desconocido";
    }

private:
    ValueType type;
    
    // Unión para almacenar los distintos tipos
    union Data {
        double number;
        char string_buffer[sizeof(std::string)];
        bool boolean;
        char range_buffer[sizeof(std::shared_ptr<RangeValue>)];
        char iterator_buffer[sizeof(std::shared_ptr<RangeIterator>)];
        char object_buffer[sizeof(ObjectPtr)];
        
        Data() {} // Constructor por defecto vacío
        ~Data() {} // Destructor vacío
    } data;
    
    friend std::ostream& operator<<(std::ostream& os, const Value& v);
};

inline std::ostream &
operator<<(std::ostream &os, const Value &v)
{
    if (v.isNumber())
    {
        os << v.asNumber();
    }
    else if (v.isBool())
    {
        os << (v.asBool() ? "true" : "false");
    }
    else if (v.isString())
    {
        os << "\"" << v.asString() << "\"";
    }
    else if (v.isRange())
    {
        os << "<range>";
    }
    else if (v.isIterable())
    {
        os << "<iterator>";
    }
    else
    {
        os << "<unknown>";
    }

    return os;
}

#endif