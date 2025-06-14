#include "cil_interpreter.hpp"
#include <regex>
#include <iostream>
#include <cctype>
#include <cmath>
#include <random>

CILInterpreter::CILInterpreter()
{
    // Inicializar estado
    program_counter = 0;
    program_running = true;
}

void CILInterpreter::loadProgram(const std::string& cil_code)
{
    std::istringstream stream(cil_code);
    std::string line;
    bool in_data = false;
    bool in_code = false;
    std::string current_section;
    
    while (std::getline(stream, line))
    {
        // Remover espacios en blanco al principio y final
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Ignorar líneas vacías y comentarios
        if (line.empty() || line[0] == '#')
            continue;
        
        if (line == ".DATA")
        {
            in_data = true;
            in_code = false;
            continue;
        }
        else if (line == ".CODE")
        {
            in_data = false;
            in_code = true;
            continue;
        }
        
        if (in_data)
        {
            parseDataSection(line);
        }
        else if (in_code)
        {
            current_section += line + "\n";
        }
    }
    
    if (in_code && !current_section.empty())
    {
        parseCodeSection(current_section);
    }
}

void CILInterpreter::parseDataSection(const std::string& line)
{
    // Parsear líneas como: str0 = "Hello World";
    std::regex data_regex("(\\w+)\\s*=\\s*\"([^\"]*)\"\\s*;");
    std::smatch match;
    
    if (std::regex_match(line, match, data_regex))
    {
        std::string label = match[1].str();
        std::string value = match[2].str();
        string_constants[label] = value;
    }
}

void CILInterpreter::parseCodeSection(const std::string& section)
{
    std::istringstream stream(section);
    std::string line;
    CILFunction* current_function = nullptr;
    bool in_function = false;
    
    while (std::getline(stream, line))
    {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty())
            continue;
        
        // Detectar inicio de función
        std::regex func_regex("function\\s+(\\w+)\\s*\\{");
        std::smatch match;
        
        if (std::regex_match(line, match, func_regex))
        {
            std::string func_name = match[1].str();
            functions[func_name] = CILFunction{};
            functions[func_name].name = func_name;
            current_function = &functions[func_name];
            in_function = true;
            continue;
        }
        
        // Detectar fin de función
        if (line == "}")
        {
            in_function = false;
            current_function = nullptr;
            continue;
        }
        
        if (in_function && current_function)
        {
            // Parsear argumentos y locales
            if (line.find("ARG ") == 0)
            {
                std::string arg_name = line.substr(4);
                if (arg_name.back() == ';')
                    arg_name.pop_back();
                current_function->args.push_back(arg_name);
                continue;
            }
            
            if (line.find("LOCAL ") == 0)
            {
                std::string local_name = line.substr(6);
                if (local_name.back() == ';')
                    local_name.pop_back();
                current_function->locals.push_back(local_name);
                continue;
            }
            
            // Parsear instrucción
            CILInstruction instr = parseInstruction(line);
            current_function->instructions.push_back(instr);
        }
        else
        {
            // Instrucción global
            CILInstruction instr = parseInstruction(line);
            global_instructions.push_back(instr);
        }
    }
}

CILInstruction CILInterpreter::parseInstruction(const std::string& line)
{
    CILInstruction instr;
    
    // Remover punto y coma final
    std::string clean_line = line;
    if (!clean_line.empty() && clean_line.back() == ';')
        clean_line.pop_back();
    
    // Detectar etiquetas (terminan en :)
    if (!clean_line.empty() && clean_line.back() == ':')
    {
        instr.type = CILInstruction::LABEL;
        instr.label = clean_line.substr(0, clean_line.length() - 1);
        return instr;
    }
    
    // Detectar RETURN
    if (clean_line.find("RETURN ") == 0)
    {
        instr.type = CILInstruction::RETURN;
        instr.args.push_back(clean_line.substr(7));
        return instr;
    }
    
    // Detectar GOTO
    if (clean_line.find("GOTO ") == 0)
    {
        instr.type = CILInstruction::GOTO;
        instr.label = clean_line.substr(5);
        return instr;
    }
    
    // Detectar IF GOTO - corregir regex para manejar números decimales
    std::regex if_goto_regex("IF\\s+(\\w+)\\s+(==|!=)\\s+([\\w\\.]+)\\s+GOTO\\s+(\\w+)");
    std::smatch match;
    if (std::regex_match(clean_line, match, if_goto_regex))
    {
        instr.type = CILInstruction::IF_GOTO;
        instr.args.push_back(match[1].str()); // variable
        instr.op = match[2].str();           // operador de comparación
        instr.args.push_back(match[3].str()); // valor de comparación
        instr.label = match[4].str();         // etiqueta
        return instr;
    }
    
    // Detectar asignaciones con =
    size_t eq_pos = clean_line.find(" = ");
    if (eq_pos != std::string::npos)
    {
        instr.dest = clean_line.substr(0, eq_pos);
        std::string rhs = clean_line.substr(eq_pos + 3);
        
        // CALL
        if (rhs.find("CALL ") == 0)
        {
            instr.type = CILInstruction::CALL;
            std::istringstream iss(rhs.substr(5));
            std::string token;
            while (iss >> token)
            {
                instr.args.push_back(token);
            }
            return instr;
        }
        
        // GETATTR
        if (rhs.find("GETATTR ") == 0)
        {
            instr.type = CILInstruction::GETATTR;
            std::istringstream iss(rhs.substr(8));
            std::string token;
            while (iss >> token)
            {
                instr.args.push_back(token);
            }
            return instr;
        }
        
        // ALLOCATE
        if (rhs.find("ALLOCATE ") == 0)
        {
            instr.type = CILInstruction::ALLOCATE;
            instr.args.push_back(rhs.substr(9));
            return instr;
        }
        
        // Operación binaria - mejorar regex para números decimales
        std::regex binop_regex("([\\w\\.]+)\\s+([+\\-*/%%\\^<>&|=!]+)\\s+([\\w\\.]+)");
        if (std::regex_match(rhs, match, binop_regex))
        {
            instr.type = CILInstruction::BINARY_OP;
            instr.args.push_back(match[1].str()); // operando izquierdo
            instr.op = match[2].str();            // operador
            instr.args.push_back(match[3].str()); // operando derecho
            return instr;
        }
        
        // Asignación simple
        instr.type = CILInstruction::ASSIGNMENT;
        instr.args.push_back(rhs);
        return instr;
    }
    
    // SETATTR
    if (clean_line.find("SETATTR ") == 0)
    {
        instr.type = CILInstruction::SETATTR;
        std::istringstream iss(clean_line.substr(8));
        std::string token;
        while (iss >> token)
        {
            instr.args.push_back(token);
        }
        return instr;
    }
    
    // Por defecto, asignación
    instr.type = CILInstruction::ASSIGNMENT;
    return instr;
}

void CILInterpreter::execute()
{
    // Buscar función main o la primera función como punto de entrada
    std::string entry_point = "main";
    if (functions.find(entry_point) == functions.end() && !functions.empty())
    {
        entry_point = functions.begin()->first;
    }
    
    if (functions.find(entry_point) != functions.end())
    {
        // Ejecutar función de entrada
        executeFunction(entry_point, {});
    }
    else
    {
        std::cerr << "No se encontró punto de entrada para la ejecución\n";
    }
}

CILValue CILInterpreter::executeFunction(const std::string& func_name, const std::vector<CILValue>& args)
{
    auto it = functions.find(func_name);
    if (it == functions.end())
    {
        throw std::runtime_error("Función no encontrada: " + func_name);
    }
    
    const auto& func = it->second;
    
    // Crear nuevo marco de activación
    ActivationFrame frame;
    frame.function_name = func_name;
    
    // Asignar argumentos
    for (size_t i = 0; i < func.args.size() && i < args.size(); i++)
    {
        frame.variables[func.args[i]] = args[i];
    }
    
    // Cambiar contexto
    call_stack.push(frame);
    current_frame = &call_stack.top();
    
    // Ejecutar instrucciones
    int pc = 0;
    bool function_running = true;
    
    while (function_running && pc < static_cast<int>(func.instructions.size()))
    {
        const auto& instr = func.instructions[pc];
        
        switch (instr.type)
        {
        case CILInstruction::ASSIGNMENT:
            executeAssignment(instr);
            break;
        case CILInstruction::BINARY_OP:
            executeBinaryOp(instr);
            break;
        case CILInstruction::CALL:
            executeCall(instr);
            break;
        case CILInstruction::RETURN:
            executeReturn(instr);
            function_running = false;
            break;
        case CILInstruction::GOTO:
            pc = findLabel(func, instr.label);
            continue; // No incrementar pc
        case CILInstruction::IF_GOTO:
            if (shouldJump(instr))
            {
                pc = findLabel(func, instr.label);
                continue; // No incrementar pc
            }
            break;
        case CILInstruction::LABEL:
            // Las etiquetas no hacen nada por sí mismas
            break;
        default:
            std::cerr << "Instrucción no implementada\n";
            break;
        }
        
        pc++;
    }
    
    // Obtener valor de retorno
    CILValue return_val = current_frame->return_value;
    
    // Restaurar contexto
    call_stack.pop();
    if (!call_stack.empty())
    {
        current_frame = &call_stack.top();
    }
    else
    {
        current_frame = nullptr;
    }
    
    return return_val;
}

int CILInterpreter::findLabel(const CILFunction& func, const std::string& label)
{
    for (size_t i = 0; i < func.instructions.size(); i++)
    {
        if (func.instructions[i].type == CILInstruction::LABEL &&
            func.instructions[i].label == label)
        {
            return static_cast<int>(i);
        }
    }
    return 0; // Si no encuentra la etiqueta, ir al inicio
}

bool CILInterpreter::shouldJump(const CILInstruction& instr)
{
    if (instr.args.size() < 2)
        return false;
    
    CILValue condition = getValue(instr.args[0]);
    CILValue compare_value = getValue(instr.args[1]);
    
    if (instr.op == "==")
    {
        return (valueToNumber(condition) == valueToNumber(compare_value));
    }
    else if (instr.op == "!=")
    {
        return (valueToNumber(condition) != valueToNumber(compare_value));
    }
    
    return false;
}

void CILInterpreter::executeInstruction(const CILInstruction& instr)
{
    switch (instr.type)
    {
    case CILInstruction::ASSIGNMENT:
        executeAssignment(instr);
        break;
    case CILInstruction::BINARY_OP:
        executeBinaryOp(instr);
        break;
    case CILInstruction::CALL:
        executeCall(instr);
        break;
    case CILInstruction::RETURN:
        executeReturn(instr);
        break;
    case CILInstruction::GOTO:
        executeGoto(instr);
        break;
    case CILInstruction::IF_GOTO:
        executeIfGoto(instr);
        break;
    case CILInstruction::LABEL:
        // Las etiquetas no hacen nada por sí mismas
        break;
    default:
        std::cerr << "Instrucción no implementada\n";
        break;
    }
}

void CILInterpreter::executeAssignment(const CILInstruction& instr)
{
    if (instr.args.empty())
        return;
    
    CILValue value = getValue(instr.args[0]);
    setValue(instr.dest, value);
}

void CILInterpreter::executeBinaryOp(const CILInstruction& instr)
{
    if (instr.args.size() < 2)
        return;
    
    CILValue left = getValue(instr.args[0]);
    CILValue right = getValue(instr.args[1]);
    CILValue result;
    
    std::string op = instr.op;
    
    if (op == "+")
    {
        result = valueToNumber(left) + valueToNumber(right);
    }
    else if (op == "-")
    {
        result = valueToNumber(left) - valueToNumber(right);
    }
    else if (op == "*")
    {
        result = valueToNumber(left) * valueToNumber(right);
    }
    else if (op == "/")
    {
        double r = valueToNumber(right);
        if (r == 0.0)
            throw std::runtime_error("División por cero");
        result = valueToNumber(left) / r;
    }
    else if (op == "%")
    {
        result = fmod(valueToNumber(left), valueToNumber(right));
    }
    else if (op == "^")
    {
        result = pow(valueToNumber(left), valueToNumber(right));
    }
    else if (op == "<")
    {
        result = valueToNumber(left) < valueToNumber(right);
    }
    else if (op == ">")
    {
        result = valueToNumber(left) > valueToNumber(right);
    }
    else if (op == "<=")
    {
        result = valueToNumber(left) <= valueToNumber(right);
    }
    else if (op == ">=")
    {
        result = valueToNumber(left) >= valueToNumber(right);
    }
    else if (op == "==")
    {
        result = valueToNumber(left) == valueToNumber(right);
    }
    else if (op == "!=")
    {
        result = valueToNumber(left) != valueToNumber(right);
    }
    else if (op == "&&")
    {
        result = valueToBool(left) && valueToBool(right);
    }
    else if (op == "||")
    {
        result = valueToBool(left) || valueToBool(right);
    }
    else if (op == "@") // Concatenación
    {
        result = valueToString(left) + valueToString(right);
    }
    else
    {
        throw std::runtime_error("Operador no soportado: " + op);
    }
    
    setValue(instr.dest, result);
}

void CILInterpreter::executeCall(const CILInstruction& instr)
{
    if (instr.args.empty())
        return;
    
    std::string func_name = instr.args[0];
    std::vector<CILValue> args;
    
    // Recopilar argumentos
    for (size_t i = 1; i < instr.args.size(); i++)
    {
        args.push_back(getValue(instr.args[i]));
    }
    
    // Verificar si es función nativa
    if (func_name == "print" || func_name == "sqrt" || func_name == "sin" || 
        func_name == "cos" || func_name == "rand" || func_name == "PI" || func_name == "E")
    {
        CILValue result = callNativeFunction(func_name, args);
        setValue(instr.dest, result);
        return;
    }
    
    // Función definida por el usuario - usar executeFunction
    CILValue result = executeFunction(func_name, args);
    setValue(instr.dest, result);
}

void CILInterpreter::executeReturn(const CILInstruction& instr)
{
    if (!instr.args.empty() && current_frame)
    {
        current_frame->return_value = getValue(instr.args[0]);
    }
}

void CILInterpreter::executeGoto(const CILInstruction& instr)
{
    // Esta función ya no se usa con el nuevo enfoque
}

void CILInterpreter::executeIfGoto(const CILInstruction& instr)
{
    // Esta función ya no se usa con el nuevo enfoque
}

CILValue CILInterpreter::getValue(const std::string& name)
{
    // Verificar si es un literal numérico
    if (isNumeric(name))
    {
        return std::stod(name);
    }
    
    // Verificar si es una constante de cadena
    if (string_constants.find(name) != string_constants.end())
    {
        return string_constants[name];
    }
    
    // Verificar en variables locales
    if (current_frame && current_frame->variables.find(name) != current_frame->variables.end())
    {
        return current_frame->variables[name];
    }
    
    // Verificar en variables globales
    if (globals.find(name) != globals.end())
    {
        return globals[name];
    }
    
    // Por defecto, retornar 0
    return 0.0;
}

void CILInterpreter::setValue(const std::string& name, const CILValue& value)
{
    if (current_frame)
    {
        current_frame->variables[name] = value;
    }
    else
    {
        globals[name] = value;
    }
}

bool CILInterpreter::isNumeric(const std::string& str)
{
    if (str.empty())
        return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+')
        start = 1;
    
    bool has_dot = false;
    for (size_t i = start; i < str.length(); i++)
    {
        if (str[i] == '.')
        {
            if (has_dot)
                return false;
            has_dot = true;
        }
        else if (!std::isdigit(str[i]))
        {
            return false;
        }
    }
    
    return str.length() > start;
}

CILValue CILInterpreter::parseValue(const std::string& str)
{
    if (isNumeric(str))
    {
        return std::stod(str);
    }
    return str;
}

CILValue CILInterpreter::callNativeFunction(const std::string& name, const std::vector<CILValue>& args)
{
    if (name == "print")
    {
        if (!args.empty())
        {
            printValue(args[0]);
            std::cout << std::endl; // Agregar salto de línea
        }
        return 0.0;
    }
    else if (name == "sqrt")
    {
        if (!args.empty())
        {
            return sqrt(valueToNumber(args[0]));
        }
        return 0.0;
    }
    else if (name == "sin")
    {
        if (!args.empty())
        {
            return sin(valueToNumber(args[0]));
        }
        return 0.0;
    }
    else if (name == "cos")
    {
        if (!args.empty())
        {
            return cos(valueToNumber(args[0]));
        }
        return 0.0;
    }
    else if (name == "rand")
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0, 1.0);
        return dis(gen);
    }
    else if (name == "PI")
    {
        return M_PI;
    }
    else if (name == "E")
    {
        return M_E;
    }
    
    return 0.0;
}

void CILInterpreter::printValue(const CILValue& value)
{
    std::visit([](const auto& v) {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>)
        {
            // Para números enteros, mostrar sin decimales
            if (v == static_cast<int>(v))
                std::cout << static_cast<int>(v);
            else
                std::cout << v;
        }
        else
        {
            std::cout << v;
        }
    }, value);
}

std::string CILInterpreter::valueToString(const CILValue& value)
{
    return std::visit([](const auto& v) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>)
            return v;
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>)
            return v ? "true" : "false";
        else
            return std::to_string(v);
    }, value);
}

double CILInterpreter::valueToNumber(const CILValue& value)
{
    return std::visit([](const auto& v) -> double {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>)
            return v;
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>)
            return v ? 1.0 : 0.0;
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>)
            return std::stod(v);
        else
            return 0.0;
    }, value);
}

bool CILInterpreter::valueToBool(const CILValue& value)
{
    return std::visit([](const auto& v) -> bool {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>)
            return v;
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>)
            return v != 0.0;
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>)
            return !v.empty();
        else
            return false;
    }, value);
}