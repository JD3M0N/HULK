#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <variant>
#include <iostream>
#include <sstream>
#include <memory>  
#include <algorithm>

// Valor en el intérprete CIL - ahora incluye punteros a objetos y arrays
using CILValue = std::variant<double, std::string, bool, void*>;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

// Estructura para objetos en memoria
struct CILObject
{
    std::string type_name;
    std::unordered_map<std::string, CILValue> fields;
    
    CILObject(const std::string& type) : type_name(type) {}
};

// Estructura para arrays en memoria
struct CILArray
{
    std::vector<CILValue> elements;
    
    CILArray(size_t size) : elements(size, 0.0) {}
};

// ← NUEVA ESTRUCTURA PARA INFORMACIÓN DE TIPOS
struct CILTypeInfo
{
    std::string name;
    std::string parent;  // Tipo padre para herencia
    std::vector<std::string> attributes;
    std::unordered_map<std::string, std::string> methods; // método -> función_label
    
    CILTypeInfo() = default;
    CILTypeInfo(const std::string& type_name) : name(type_name), parent("Object") {}
    
    bool hasAttribute(const std::string& attr) const
    {
        return std::find(attributes.begin(), attributes.end(), attr) != attributes.end();
    }
    
    bool hasMethod(const std::string& method) const
    {
        return methods.find(method) != methods.end();
    }
    
    std::string getMethodLabel(const std::string& method) const
    {
        auto it = methods.find(method);
        return (it != methods.end()) ? it->second : "";
    }
};

// Instrucción CIL - agregamos nuevos tipos
struct CILInstruction
{
    enum Type
    {
        ASSIGNMENT,     // x = y
        BINARY_OP,      // x = y + z
        CALL,           // x = CALL func arg1 arg2
        VCALL,          // x = VCALL type method (llamada virtual)
        PARAM,          // PARAM value (pasar parámetro)
        GETATTR,        // x = GETATTR obj attr
        SETATTR,        // SETATTR obj attr value
        GETINDEX,       // x = GETINDEX arr index
        SETINDEX,       // SETINDEX arr index value
        ALLOCATE,       // x = ALLOCATE type
        ARRAY,          // x = ARRAY size
        TYPEOF,         // x = TYPEOF obj
        LOAD,           // x = LOAD string_label
        LENGTH,         // x = LENGTH string
        CONCAT,         // x = CONCAT str1 str2
        SUBSTRING,      // x = SUBSTRING str start length
        STR,            // x = STR number
        read_INSTR,     // x = READ (cambiar nombre para evitar conflicto)
        PRINT,          // PRINT value
        RETURN,         // RETURN value
        GOTO,           // GOTO label
        IF_GOTO,        // IF condition GOTO label
        LABEL           // label:
    };
    
    Type type;
    std::string dest;        // Variable destino
    std::string op;          // Operador o función
    std::vector<std::string> args; // Argumentos
    std::string label;       // Para GOTO y LABEL
};

// Función CIL
struct CILFunction
{
    std::string name;
    std::vector<std::string> args;
    std::vector<std::string> locals;
    std::vector<CILInstruction> instructions;
    int start_line = 0; // Línea donde comienza la función
};

// Marco de activación - agregamos parámetros
struct ActivationFrame
{
    std::string function_name;
    std::unordered_map<std::string, CILValue> variables;
    std::vector<CILValue> parameters; // Parámetros pasados con PARAM
    int return_address = -1; // Dirección de retorno
    CILValue return_value; // Valor de retorno
    
    ActivationFrame() : return_value(0.0) {}
};

class CILInterpreter
{
private:
    // Datos del programa
    std::unordered_map<std::string, std::string> string_constants;
    std::unordered_map<std::string, CILFunction> functions;
    std::vector<CILInstruction> global_instructions;
    
    // ← NUEVA TABLA DE TIPOS
    std::unordered_map<std::string, CILTypeInfo> type_table;
    std::string current_parsing_type;  // Para trackear el tipo que se está parseando
    
    // Gestión de memoria
    std::vector<std::unique_ptr<CILObject>> objects;
    std::vector<std::unique_ptr<CILArray>> arrays;
    
    // Estado de ejecución
    std::stack<ActivationFrame> call_stack;
    ActivationFrame* current_frame = nullptr;
    int program_counter = 0;
    bool program_running = true;
    
    // Variables globales
    std::unordered_map<std::string, CILValue> globals;
    
public:
    CILInterpreter();
    
    // Cargar y ejecutar programa CIL
    void loadProgram(const std::string& cil_code);
    void execute();
    
    // Ejecutar función específica
    CILValue executeFunction(const std::string& func_name, const std::vector<CILValue>& args);
    
    // Métodos auxiliares
    void parseDataSection(const std::string& section);
    void parseTypesSection(const std::string& line);  // ← DECLARACIÓN ACTUALIZADA
    void parseCodeSection(const std::string& section);
    CILInstruction parseInstruction(const std::string& line);
    
    // ← NUEVOS MÉTODOS PARA MANEJO DE TIPOS
    bool typeExists(const std::string& type_name) const;
    const CILTypeInfo& getTypeInfo(const std::string& type_name) const;
    std::string resolveMethodCall(const std::string& type_name, const std::string& method_name) const;
    
    // Control de flujo
    int findLabel(const CILFunction& func, const std::string& label);
    bool shouldJump(const CILInstruction& instr);
    
    // Ejecución de instrucciones
    void executeInstruction(const CILInstruction& instr);
    void executeAssignment(const CILInstruction& instr);
    void executeBinaryOp(const CILInstruction& instr);
    void executeCall(const CILInstruction& instr);
    void executeVCall(const CILInstruction& instr);
    void executeParam(const CILInstruction& instr);
    void executeAllocate(const CILInstruction& instr);
    void executeArray(const CILInstruction& instr);
    void executeTypeOf(const CILInstruction& instr);
    void executeLoad(const CILInstruction& instr);
    void executeLength(const CILInstruction& instr);
    void executeConcat(const CILInstruction& instr);
    void executeSubstring(const CILInstruction& instr);
    void executeStr(const CILInstruction& instr);
    void executeRead(const CILInstruction& instr);
    void executePrint(const CILInstruction& instr);
    void executeGetIndex(const CILInstruction& instr);
    void executeSetIndex(const CILInstruction& instr);
    void executeReturn(const CILInstruction& instr);
    void executeGetAttr(const CILInstruction& instr);     // ← NUEVO MÉTODO
    void executeSetAttr(const CILInstruction& instr);     // ← NUEVO MÉTODO
    
    // Manejo de valores
    CILValue getValue(const std::string& name);
    void setValue(const std::string& name, const CILValue& value);
    bool isNumeric(const std::string& str);
    CILValue parseValue(const std::string& str);
    
    // Manejo de memoria
    CILObject* getObject(void* ptr);
    CILArray* getArray(void* ptr);
    
    // Funciones nativas
    CILValue callNativeFunction(const std::string& name, const std::vector<CILValue>& args);
    
    // Utilidades
    void printValue(const CILValue& value);
    std::string valueToString(const CILValue& value);
    double valueToNumber(const CILValue& value);
    bool valueToBool(const CILValue& value);
};