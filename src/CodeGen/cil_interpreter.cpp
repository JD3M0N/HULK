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
    bool in_types = false;  // ← NUEVA BANDERA
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
        
        if (line == ".TYPES")  // ← MANEJAR NUEVA SECCIÓN
        {
            in_types = true;
            in_data = false;
            in_code = false;
            continue;
        }
        else if (line == ".DATA")
        {
            in_types = false;
            in_data = true;
            in_code = false;
            continue;
        }
        else if (line == ".CODE")
        {
            in_types = false;
            in_data = false;
            in_code = true;
            continue;
        }
        
        if (in_types)
        {
            parseTypesSection(line);  // ← NUEVO MÉTODO
        }
        else if (in_data)
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

void CILInterpreter::parseTypesSection(const std::string& line)
{
    // Parsear declaraciones de tipos según especificación del libro
    // Ejemplos:
    // type Main {
    //     attribute Main_msg ;
    //     method Main_main: f1 ;
    // }
    
    static std::regex type_regex("type\\s+(\\w+)\\s*\\{");
    static std::regex attr_regex("\\s*attribute\\s+(\\w+)\\s*;");
    static std::regex method_regex("\\s*method\\s+(\\w+):\\s*(\\w+)\\s*;");
    static std::regex inherits_regex("type\\s+(\\w+)\\s*inherits\\s+(\\w+)\\s*\\{");
    static std::regex close_brace_regex("\\s*\\}\\s*");
    
    std::smatch match;
    
    // 1. Detectar inicio de declaración de tipo con herencia
    if (std::regex_match(line, match, inherits_regex))
    {
        std::string type_name = match[1].str();
        std::string parent_name = match[2].str();
        
        current_parsing_type = type_name;
        type_table[type_name] = CILTypeInfo(type_name);
        type_table[type_name].parent = parent_name;
        
        std::cout << "Registrando tipo: " << type_name << " (hereda de " << parent_name << ")" << std::endl;
        return;
    }
    
    // 2. Detectar inicio de declaración de tipo simple
    if (std::regex_match(line, match, type_regex))
    {
        std::string type_name = match[1].str();
        current_parsing_type = type_name;
        type_table[type_name] = CILTypeInfo(type_name);
        
        std::cout << "Registrando tipo: " << type_name << std::endl;
        return;
    }
    
    // 3. Detectar fin de declaración de tipo
    if (std::regex_match(line, close_brace_regex))
    {
        if (!current_parsing_type.empty())
        {
            std::cout << "Finalizando tipo: " << current_parsing_type << std::endl;
            current_parsing_type.clear();
        }
        return;
    }
    
    // Si estamos dentro de una declaración de tipo, procesar atributos y métodos
    if (!current_parsing_type.empty())
    {
        // 4. Detectar atributos
        if (std::regex_match(line, match, attr_regex))
        {
            std::string full_attr_name = match[1].str();
            type_table[current_parsing_type].attributes.push_back(full_attr_name);
            
            std::cout << "  - Atributo: " << full_attr_name << std::endl;
            return;
        }
        
        // 5. Detectar métodos
        if (std::regex_match(line, match, method_regex))
        {
            std::string full_method_name = match[1].str();
            std::string function_label = match[2].str();
            
            // Extraer el nombre del método sin el prefijo del tipo
            std::string method_name = full_method_name;
            std::string type_prefix = current_parsing_type + "_";
            if (full_method_name.find(type_prefix) == 0)
            {
                method_name = full_method_name.substr(type_prefix.length());
            }
            
            type_table[current_parsing_type].methods[method_name] = function_label;
            
            std::cout << "  - Método: " << method_name << " -> " << function_label 
                     << " (nombre completo: " << full_method_name << ")" << std::endl;
            return;
        }
    }
    
    // Si llegamos aquí, la línea no coincide con ningún patrón conocido
    // Esto es normal para líneas vacías o comentarios dentro de las declaraciones de tipo
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

// Actualizar parseInstruction para manejar mejor las instrucciones según especificación

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
    if (clean_line.find("RETURN") == 0)
    {
        instr.type = CILInstruction::RETURN;
        if (clean_line.length() > 6 && clean_line[6] == ' ')
        {
            instr.args.push_back(clean_line.substr(7));
        }
        return instr;
    }
    
    // Detectar GOTO
    if (clean_line.find("GOTO ") == 0)
    {
        instr.type = CILInstruction::GOTO;
        instr.label = clean_line.substr(5);
        return instr;
    }
    
    // Detectar PARAM
    if (clean_line.find("PARAM ") == 0)
    {
        instr.type = CILInstruction::PARAM;
        instr.args.push_back(clean_line.substr(6));
        return instr;
    }
    
    // Detectar PRINT (sin asignación)
    if (clean_line.find("PRINT ") == 0)
    {
        instr.type = CILInstruction::PRINT;
        instr.args.push_back(clean_line.substr(6));
        return instr;
    }
    
    // Detectar IF GOTO - MEJORADO PARA MANEJAR COMPARACIONES
    std::regex if_goto_regex("IF\\s+([\\w\\.]+)\\s+(==|!=|<|>|<=|>=)\\s+([\\w\\.\\-]+)\\s+GOTO\\s+(\\w+)");
    std::smatch match;
    if (std::regex_match(clean_line, match, if_goto_regex))
    {
        instr.type = CILInstruction::IF_GOTO;
        instr.args.push_back(match[1].str()); // variable izquierda
        instr.op = match[2].str();           // operador de comparación
        instr.args.push_back(match[3].str()); // variable/valor derecha
        instr.label = match[4].str();         // etiqueta
        return instr;
    }
    
    // Fallback: detectar IF GOTO simple (solo condición)
    std::regex if_goto_simple("IF\\s+(\\w+)\\s+GOTO\\s+(\\w+)");
    if (std::regex_match(clean_line, match, if_goto_simple))
    {
        instr.type = CILInstruction::IF_GOTO;
        instr.args.push_back(match[1].str());
        instr.label = match[2].str();
        return instr;
    }
    
    // Detectar asignaciones con =
    size_t eq_pos = clean_line.find(" = ");
    if (eq_pos != std::string::npos)
    {
        instr.dest = clean_line.substr(0, eq_pos);
        std::string rhs = clean_line.substr(eq_pos + 3);
        
        // READ
        if (rhs == "READ")
        {
            instr.type = CILInstruction::read_INSTR;
            return instr;
        }
        
        // LOAD - MEJORADO
        if (rhs.find("LOAD ") == 0)
        {
            instr.type = CILInstruction::LOAD;
            instr.args.push_back(rhs.substr(5));
            return instr;
        }
        
        // CALL - MEJORADO para manejar múltiples argumentos
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
        
        // VCALL - NUEVO
        if (rhs.find("VCALL ") == 0)
        {
            instr.type = CILInstruction::VCALL;
            std::istringstream iss(rhs.substr(6));
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
        
        // ARRAY
        if (rhs.find("ARRAY ") == 0)
        {
            instr.type = CILInstruction::ARRAY;
            instr.args.push_back(rhs.substr(6));
            return instr;
        }
        
        // TYPEOF
        if (rhs.find("TYPEOF ") == 0)
        {
            instr.type = CILInstruction::TYPEOF;
            instr.args.push_back(rhs.substr(7));
            return instr;
        }
        
        // LENGTH, CONCAT, SUBSTRING, STR (según especificación)
        if (rhs.find("LENGTH ") == 0)
        {
            instr.type = CILInstruction::LENGTH;
            instr.args.push_back(rhs.substr(7));
            return instr;
        }
        
        if (rhs.find("CONCAT ") == 0)
        {
            instr.type = CILInstruction::CONCAT;
            std::istringstream iss(rhs.substr(7));
            std::string token;
            while (iss >> token)
            {
                instr.args.push_back(token);
            }
            return instr;
        }
        
        if (rhs.find("SUBSTRING ") == 0)
        {
            instr.type = CILInstruction::SUBSTRING;
            std::istringstream iss(rhs.substr(10));
            std::string token;
            while (iss >> token)
            {
                instr.args.push_back(token);
            }
            return instr;
        }
        
        if (rhs.find("STR ") == 0)
        {
            instr.type = CILInstruction::STR;
            instr.args.push_back(rhs.substr(4));
            return instr;
        }
        
        // GETINDEX y GETATTR
        if (rhs.find("GETINDEX ") == 0)
        {
            instr.type = CILInstruction::GETINDEX;
            std::istringstream iss(rhs.substr(9));
            std::string token;
            while (iss >> token)
            {
                instr.args.push_back(token);
            }
            return instr;
        }
        
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
        
        // Operación binaria - MEJORADO
        std::regex binop_regex("([\\w\\.\\-]+)\\s+([+\\-*/%%\\^<>&|=!]+)\\s+([\\w\\.\\-]+)");
        if (std::regex_match(rhs, match, binop_regex))
        {
            instr.type = CILInstruction::BINARY_OP;
            instr.args.push_back(match[1].str());
            instr.op = match[2].str();
            instr.args.push_back(match[3].str());
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
    
    // SETINDEX
    if (clean_line.find("SETINDEX ") == 0)
    {
        instr.type = CILInstruction::SETINDEX;
        std::istringstream iss(clean_line.substr(9));
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
    // Buscar función entry primero, luego main, o la primera función
    std::string entry_point = "entry";  // ← CAMBIAR PRIORIDAD
    if (functions.find(entry_point) == functions.end())
    {
        entry_point = "main";
        if (functions.find(entry_point) == functions.end() && !functions.empty())
        {
            entry_point = functions.begin()->first;
        }
    }
    
    if (functions.find(entry_point) != functions.end())
    {
        std::cout << "Ejecutando función: " << entry_point << std::endl;
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

// Actualizar shouldJump para evaluar comparaciones correctamente

bool CILInterpreter::shouldJump(const CILInstruction& instr)
{
    if (instr.args.empty())
        return false;
    
    // Si hay operador de comparación, evaluarlo
    if (!instr.op.empty() && instr.args.size() >= 2)
    {
        CILValue left = getValue(instr.args[0]);
        CILValue right = getValue(instr.args[1]);
        
        double left_num = valueToNumber(left);
        double right_num = valueToNumber(right);
        
        if (instr.op == "==")
            return left_num == right_num;
        else if (instr.op == "!=")
            return left_num != right_num;
        else if (instr.op == "<")
            return left_num < right_num;
        else if (instr.op == ">")
            return left_num > right_num;
        else if (instr.op == "<=")
            return left_num <= right_num;
        else if (instr.op == ">=")
            return left_num >= right_num;
        
        return false;
    }
    
    // Caso simple: evaluar solo la condición
    CILValue condition = getValue(instr.args[0]);
    return valueToBool(condition);
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
    case CILInstruction::VCALL:
        executeVCall(instr);
        break;
    case CILInstruction::PARAM:
        executeParam(instr);
        break;
    case CILInstruction::GETATTR:        // ← NUEVO CASO
        executeGetAttr(instr);
        break;
    case CILInstruction::SETATTR:        // ← NUEVO CASO
        executeSetAttr(instr);
        break;
    case CILInstruction::ALLOCATE:
        executeAllocate(instr);
        break;
    case CILInstruction::ARRAY:
        executeArray(instr);
        break;
    case CILInstruction::TYPEOF:
        executeTypeOf(instr);
        break;
    case CILInstruction::LOAD:
        executeLoad(instr);
        break;
    case CILInstruction::LENGTH:
        executeLength(instr);
        break;
    case CILInstruction::CONCAT:
        executeConcat(instr);
        break;
    case CILInstruction::SUBSTRING:
        executeSubstring(instr);
        break;
    case CILInstruction::STR:
        executeStr(instr);
        break;
    case CILInstruction::read_INSTR: 
        executeRead(instr);
        break;
    case CILInstruction::PRINT:
        executePrint(instr);
        break;
    case CILInstruction::GETINDEX:
        executeGetIndex(instr);
        break;
    case CILInstruction::SETINDEX:
        executeSetIndex(instr);
        break;
    case CILInstruction::RETURN:
        executeReturn(instr);
        break;
    case CILInstruction::LABEL:
        // Las etiquetas no hacen nada por sí mismas
        break;
    default:
        std::cerr << "Instrucción no implementada: " << static_cast<int>(instr.type) << std::endl;
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
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, void*>)
        {
            // Para punteros, mostrar dirección
            std::cout << "Object@"<< v;
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
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, void*>)
            return "Object@" + std::to_string(reinterpret_cast<uintptr_t>(v));
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
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, void*>)
            return reinterpret_cast<uintptr_t>(v);
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
        else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, void*>)
            return v != nullptr;
        else
            return false;
    }, value);
}

// Implementaciones de los métodos adicionales

void CILInterpreter::executeAllocate(const CILInstruction& instr)
{
    if (instr.args.empty())
        return;
    
    std::string type_name = instr.args[0];
    auto obj = std::make_unique<CILObject>(type_name);
    void* ptr = obj.get();
    objects.push_back(std::move(obj));
    
    setValue(instr.dest, ptr);
}

void CILInterpreter::executeArray(const CILInstruction& instr)
{
    if (instr.args.empty())
        return;
    
    CILValue size_val = getValue(instr.args[0]);
    size_t size = static_cast<size_t>(valueToNumber(size_val));
    
    auto arr = std::make_unique<CILArray>(size);
    void* ptr = arr.get();
    arrays.push_back(std::move(arr));
    
    setValue(instr.dest, ptr);
}

void CILInterpreter::executeTypeOf(const CILInstruction& instr)
{
    if (instr.args.empty())
        return;
    
    CILValue obj_val = getValue(instr.args[0]);
    if (std::holds_alternative<void*>(obj_val))
    {
        void* ptr = std::get<void*>(obj_val);
        CILObject* obj = getObject(ptr);
        if (obj)
        {
            setValue(instr.dest, obj->type_name);
            return;
        }
    }
    
    setValue(instr.dest, std::string("Object"));
}

void CILInterpreter::executeLoad(const CILInstruction& instr)
{
    if (instr.args.empty())
        return;
    
    std::string label = instr.args[0];
    if (string_constants.find(label) != string_constants.end())
    {
        setValue(instr.dest, string_constants[label]);
    }
    else
    {
        setValue(instr.dest, std::string(""));
    }
}

void CILInterpreter::executeLength(const CILInstruction& instr)
{
    if (instr.args.empty())
        return;
    
    CILValue str_val = getValue(instr.args[0]);
    std::string str = valueToString(str_val);
    setValue(instr.dest, static_cast<double>(str.length()));
}

void CILInterpreter::executeConcat(const CILInstruction& instr)
{
    if (instr.args.size() < 2)
        return;
    
    CILValue str1_val = getValue(instr.args[0]);
    CILValue str2_val = getValue(instr.args[1]);
    
    std::string result = valueToString(str1_val) + valueToString(str2_val);
    setValue(instr.dest, result);
}

void CILInterpreter::executeSubstring(const CILInstruction& instr)
{
    if (instr.args.size() < 3)
        return;
    
    CILValue str_val = getValue(instr.args[0]);
    CILValue start_val = getValue(instr.args[1]);
    CILValue length_val = getValue(instr.args[2]);
    
    std::string str = valueToString(str_val);
    size_t start = static_cast<size_t>(valueToNumber(start_val));
    size_t length = static_cast<size_t>(valueToNumber(length_val));
    
    if (start < str.length())
    {
        std::string result = str.substr(start, length);
        setValue(instr.dest, result);
    }
    else
    {
        setValue(instr.dest, std::string(""));
    }
}

void CILInterpreter::executeStr(const CILInstruction& instr)
{
    if (instr.args.empty())
        return;
    
    CILValue num_val = getValue(instr.args[0]);
    double num = valueToNumber(num_val);
    
    std::string result;
    if (num == static_cast<int>(num))
    {
        result = std::to_string(static_cast<int>(num));
    }
    else
    {
        result = std::to_string(num);
    }
    
    setValue(instr.dest, result);
}

void CILInterpreter::executeRead(const CILInstruction& instr)
{
    std::string line;
    std::getline(std::cin, line);
    setValue(instr.dest, line);
}

void CILInterpreter::executePrint(const CILInstruction& instr)
{
    if (!instr.args.empty())
    {
        CILValue value = getValue(instr.args[0]);
        printValue(value);
        // No agregar salto de línea según especificación
    }
}

void CILInterpreter::executeGetIndex(const CILInstruction& instr)
{
    if (instr.args.size() < 2)
        return;
    
    CILValue arr_val = getValue(instr.args[0]);
    CILValue index_val = getValue(instr.args[1]);
    
    if (std::holds_alternative<void*>(arr_val))
    {
        void* ptr = std::get<void*>(arr_val);
        CILArray* arr = getArray(ptr);
        if (arr)
        {
            size_t index = static_cast<size_t>(valueToNumber(index_val));
            if (index < arr->elements.size())
            {
                setValue(instr.dest, arr->elements[index]);
                return;
            }
        }
    }
    
    setValue(instr.dest, 0.0);
}

void CILInterpreter::executeSetIndex(const CILInstruction& instr)
{
    if (instr.args.size() < 3)
        return;
    
    CILValue arr_val = getValue(instr.args[0]);
    CILValue index_val = getValue(instr.args[1]);
    CILValue value_val = getValue(instr.args[2]);
    
    if (std::holds_alternative<void*>(arr_val))
    {
        void* ptr = std::get<void*>(arr_val);
        CILArray* arr = getArray(ptr);
        if (arr)
        {
            size_t index = static_cast<size_t>(valueToNumber(index_val));
            if (index < arr->elements.size())
            {
                arr->elements[index] = value_val;
            }
        }
    }
}

void CILInterpreter::executeParam(const CILInstruction& instr)
{
    if (instr.args.empty() || !current_frame)
        return;
    
    CILValue param_value = getValue(instr.args[0]);
    current_frame->parameters.push_back(param_value);
}

void CILInterpreter::executeVCall(const CILInstruction& instr)
{
    if (instr.args.size() < 2)
        return;
    
    std::string type_name = instr.args[0];
    std::string method_name = instr.args[1];
    
    // Resolver la llamada al método usando la tabla de tipos
    std::string function_label;
    try
    {
        function_label = resolveMethodCall(type_name, method_name);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error resolviendo método " << type_name << "::" << method_name 
                 << " - " << e.what() << std::endl;
        // Fallback al comportamiento anterior
        function_label = type_name + "_" + method_name;
    }
    
    if (current_frame)
    {
        std::vector<CILValue> call_args = current_frame->parameters;
        current_frame->parameters.clear(); // Limpiar parámetros después de usar
        
        std::cout << "Llamando método virtual: " << type_name << "::" << method_name 
                 << " -> " << function_label << std::endl;
        
        CILValue result = executeFunction(function_label, call_args);
        setValue(instr.dest, result);
    }
}

CILObject* CILInterpreter::getObject(void* ptr)
{
    for (auto& obj : objects)
    {
        if (obj.get() == ptr)
            return obj.get();
    }
    return nullptr;
}

CILArray* CILInterpreter::getArray(void* ptr)
{
    for (auto& arr : arrays)
    {
        if (arr.get() == ptr)
            return arr.get();
    }
    return nullptr;
}

// Agregar implementaciones de los nuevos métodos de tipo

bool CILInterpreter::typeExists(const std::string& type_name) const
{
    return type_table.find(type_name) != type_table.end();
}

const CILTypeInfo& CILInterpreter::getTypeInfo(const std::string& type_name) const
{
    auto it = type_table.find(type_name);
    if (it == type_table.end())
    {
        throw std::runtime_error("Tipo no encontrado: " + type_name);
    }
    return it->second;
}

std::string CILInterpreter::resolveMethodCall(const std::string& type_name, const std::string& method_name) const
{
    // Buscar el método en el tipo, con soporte para herencia
    std::string current_type = type_name;
    
    while (!current_type.empty() && current_type != "Object")
    {
        if (typeExists(current_type))
        {
            const auto& type_info = getTypeInfo(current_type);
            if (type_info.hasMethod(method_name))
            {
                return type_info.getMethodLabel(method_name);
            }
            
            // Buscar en el tipo padre
            current_type = type_info.parent;
        }
        else
        {
            break;
        }
    }
    
    // Si no se encuentra, intentar con el nombre completo del método
    std::string full_method_name = type_name + "_" + method_name;
    return full_method_name;
}

// Implementar métodos para manejo de atributos

void CILInterpreter::executeGetAttr(const CILInstruction& instr)
{
    if (instr.args.size() < 2)
        return;
    
    CILValue obj_val = getValue(instr.args[0]);
    std::string attr_name = instr.args[1];
    
    if (std::holds_alternative<void*>(obj_val))
    {
        void* ptr = std::get<void*>(obj_val);
        CILObject* obj = getObject(ptr);
        if (obj)
        {
            // Verificar que el atributo existe en el tipo
            if (typeExists(obj->type_name))
            {
                const auto& type_info = getTypeInfo(obj->type_name);
                if (!type_info.hasAttribute(attr_name))
                {
                    std::cerr << "Advertencia: Atributo " << attr_name 
                             << " no declarado en tipo " << obj->type_name << std::endl;
                }
            }
            
            // Obtener el valor del atributo
            auto it = obj->fields.find(attr_name);
            if (it != obj->fields.end())
            {
                setValue(instr.dest, it->second);
            }
            else
            {
                // Atributo no inicializado, devolver valor por defecto
                setValue(instr.dest, 0.0);
            }
            return;
        }
    }
    
    // Error: no es un objeto válido
    std::cerr << "Error: GETATTR en objeto inválido" << std::endl;
    setValue(instr.dest, 0.0);
}

void CILInterpreter::executeSetAttr(const CILInstruction& instr)
{
    if (instr.args.size() < 3)
        return;
    
    CILValue obj_val = getValue(instr.args[0]);
    std::string attr_name = instr.args[1];
    CILValue new_value = getValue(instr.args[2]);
    
    if (std::holds_alternative<void*>(obj_val))
    {
        void* ptr = std::get<void*>(obj_val);
        CILObject* obj = getObject(ptr);
        if (obj)
        {
            // Verificar que el atributo existe en el tipo
            if (typeExists(obj->type_name))
            {
                const auto& type_info = getTypeInfo(obj->type_name);
                if (!type_info.hasAttribute(attr_name))
                {
                    std::cerr << "Advertencia: Atributo " << attr_name 
                             << " no declarado en tipo " << obj->type_name << std::endl;
                }
            }
            
            // Establecer el valor del atributo
            obj->fields[attr_name] = new_value;
            std::cout << "SETATTR: " << obj->type_name << "." << attr_name 
                     << " = " << valueToString(new_value) << std::endl;
            return;
        }
    }
    
    // Error: no es un objeto válido
    std::cerr << "Error: SETATTR en objeto inválido" << std::endl;
}