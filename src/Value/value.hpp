// value.hpp
#pragma once

#ifndef VALUE_HPP
#define VALUE_HPP

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <variant>

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
    using Storage = std::variant<double, std::string, bool, std::shared_ptr<RangeValue>,
                                 std::shared_ptr<RangeIterator>,
                                 ObjectPtr>;

    Value() : val(0.0) {}
    Value(double d) : val(d) {}
    Value(const std::string &s) : val(s) {}
    Value(bool b) : val(b) {}
    Value(std::shared_ptr<RangeValue> rv) : val(rv) {}
    Value(std::shared_ptr<RangeIterator> it) : val(it) {}
    Value(ObjectPtr obj) : val(std::move(obj)) {}

    ~Value() = default;

    bool
    isNumber() const
    {
        return std::holds_alternative<double>(val);
    }
    bool
    isString() const
    {
        return std::holds_alternative<std::string>(val);
    }
    bool
    isBool() const
    {
        return std::holds_alternative<bool>(val);
    }
    bool
    isRange() const
    {
        return std::holds_alternative<std::shared_ptr<RangeValue>>(val);
    }
    bool
    isIterable() const
    {
        return std::holds_alternative<std::shared_ptr<RangeIterator>>(val);
    }

    bool isObject() const 
    {
        return std::holds_alternative<ObjectPtr>(val); 
    }

    ObjectPtr 
    asObject() const 
    {
        return std::get<ObjectPtr>(val); 
    }

    double
    asNumber() const
    {
        return std::get<double>(val);
    }
    const std::string &
    asString() const
    {
        return std::get<std::string>(val);
    }
    bool
    asBool() const
    {
        return std::get<bool>(val);
    }
    std::shared_ptr<RangeValue>
    asRange() const
    {
        if (!isRange())
            throw std::runtime_error("Value no es RangeValue");
        return std::get<std::shared_ptr<RangeValue>>(val);
    }
    std::shared_ptr<RangeIterator>
    asIterable() const
    {
        if (!isIterable())
            throw std::runtime_error("Value no es RangeIterator");
        return std::get<std::shared_ptr<RangeIterator>>(val);
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

private:
    Storage val;
    friend std::ostream &operator<<(std::ostream &os, const Value &v);
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