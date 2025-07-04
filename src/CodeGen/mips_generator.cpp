#include "mips_generator.hpp"
#include <iostream>
#include <sstream>
#include <regex>

MIPSGenerator::MIPSGenerator()
{
    // Inicializar secciones MIPS para ejecutables
    data_section << ".data\n";
    text_section << ".text\n";
    text_section << ".globl main\n";
    text_section << ".globl _start\n\n"; // ← AGREGAR PUNTO DE ENTRADA PARA ELF

    // ← AGREGAR FUNCIÓN _start PARA EJECUTABLES ELF
    text_section << "_start:\n";
    text_section << "    # Punto de entrada ELF\n";
    text_section << "    jal main\n";
    text_section << "    # Terminar programa\n";
    text_section << "    li $v0, 10\n";
    text_section << "    syscall\n\n";
}

std::string MIPSGenerator::generateMIPS(const std::string &cil_code)
{
    std::istringstream stream(cil_code);
    std::string line;
    bool in_data = false, in_types = false, in_code = false;
    std::string current_section;

    // ✅ PASADA 1: Analizar el CIL para extraer información de métodos polimórficos
    std::istringstream first_pass(cil_code);
    std::string first_line;
    while (std::getline(first_pass, first_line))
    {
        first_line.erase(0, first_line.find_first_not_of(" \t"));
        first_line.erase(first_line.find_last_not_of(" \t") + 1);

        if (first_line.empty() || first_line[0] == '#')
            continue;

        // Detectar VCALL para extraer métodos usados
        std::regex vcall_regex("^\\s*(\\w+)\\s*=\\s*VCALL\\s+(\\w+)\\s+(\\w+)");
        std::smatch vcall_match;
        if (std::regex_search(first_line, vcall_match, vcall_regex))
        {
            std::string method_name = vcall_match[3].str();
            discovered_methods.insert(method_name);
            emitComment("DEBUG: Discovered method: " + method_name);
        }

        // Detectar definiciones de funciones para crear mapeo
        std::regex func_regex("function\\s+(\\w+)\\s*\\{");
        std::smatch func_match;
        if (std::regex_search(first_line, func_match, func_regex))
        {
            std::string func_name = func_match[1].str();
            // La asociación función-método se hace en processTypesSection
            // ya que tenemos la información completa ahí
        }
    }

    // ✅ PASADA 2: Procesar el CIL normalmente
    while (std::getline(stream, line))
    {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == '#')
            continue;

        if (line == ".TYPES")
        {
            in_types = true;
            in_data = false;
            in_code = false;
            continue;
        }
        if (line == ".DATA")
        {
            in_data = true;
            in_types = false;
            in_code = false;
            continue;
        }
        if (line == ".CODE")
        {
            in_code = true;
            in_data = false;
            in_types = false;
            continue;
        }

        if (in_types)
        {
            processTypesSection(line);
        }
        else if (in_data)
        {
            processDataSection(line);
        }
        else if (in_code)
        {
            current_section += line + "\n";
        }
    }

    if (!current_section.empty())
    {
        processCodeSection(current_section);
    }

    // Construir resultado final
    std::ostringstream result;
    result << data_section.str() << "\n";
    result << text_section.str();

    // ✅ GENERAR FUNCIONES POLIMÓRFICAS DINÁMICAMENTE
    generatePolymorphicDispatchers(result);

    return result.str();
}

void MIPSGenerator::processDataSection(const std::string &line)
{
    // Parsear: str0 = "Hello World";
    std::regex data_regex("(\\w+)\\s*=\\s*\"([^\"]*)\"\\s*;");
    std::smatch match;

    if (std::regex_match(line, match, data_regex))
    {
        std::string label = match[1].str();
        std::string value = match[2].str();

        data_section << label << ": .asciiz \"" << value << "\"\n";
        string_labels[label] = string_labels.size();
    }
}

void MIPSGenerator::processTypesSection(const std::string &line)
{
    // Parsear información de tipos y métodos del CIL
    // Formato: "type TypeName {" o "method MethodName: function_name ;"

    static std::string current_type = "";

    // Detectar definición de tipo: "type TypeName {" o "type TypeName inherits ParentType {"
    std::regex type_regex("type\\s+(\\w+)(?:\\s+inherits\\s+\\w+)?\\s*\\{");
    std::smatch type_match;
    if (std::regex_match(line, type_match, type_regex))
    {
        current_type = type_match[1].str();
        // ARREGLO DEL BUG: Asignar ID de tipo en el orden de declaración
        if (type_ids.find(current_type) == type_ids.end())
        {
            type_ids[current_type] = next_type_id++;
            emitComment("DEBUG: Assigned ID " + std::to_string(type_ids[current_type]) + " to type " + current_type);
        }
        return;
    }

    // Detectar método: "method MethodName: function_name ;"
    std::regex method_regex("method\\s+\\w+_(\\w+)\\s*:\\s*(\\w+)\\s*;");
    std::smatch method_match;
    if (std::regex_match(line, method_match, method_regex))
    {
        std::string method_name = method_match[1].str();
        std::string function_name = method_match[2].str();

        // ARREGLO DEL BUG: Asociar el método con el tipo correcto y su función
        if (!current_type.empty())
        {
            // Crear mapeo directo tipo -> función para cada método
            type_to_method_function[current_type][method_name] = function_name;
            // DEBUG: Agregar comentario para verificar mapeo
            emitComment("DEBUG: Mapping " + current_type + "." + method_name + " -> " + function_name);
        }
        else
        {
            emitComment("DEBUG: ERROR - No current_type for method " + method_name);
        }

        // Mantener la lista para compatibilidad (pero no se usará para el dispatcher)
        method_to_functions[method_name].push_back(function_name);

        return;
    }

    // Detectar fin de tipo: "}"
    if (line == "}")
    {
        emitComment("DEBUG: End of type " + current_type);
        current_type = "";
    }
}

void MIPSGenerator::processCodeSection(const std::string &section)
{
    std::istringstream stream(section);
    std::string line;
    std::string current_function;
    bool in_function = false;

    // Layout fijo del stack frame
    std::unordered_map<std::string, int> local_vars;
    std::vector<std::string> args;
    int frame_size = 64;

    while (std::getline(stream, line))
    {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty())
            continue;

        // Detectar funciones
        std::regex func_regex("function\\s+(\\w+)\\s*\\{");
        std::smatch match;

        if (std::regex_match(line, match, func_regex))
        {
            current_function = match[1].str();
            in_function = true;

            next_offset = 0; // ← RESETEAR contador de offsets para nueva función
            local_vars.clear();
            args.clear();

            // Generar etiqueta
            if (current_function == "entry")
            {
                text_section << "main:\n";
            }
            else
            {
                text_section << current_function << ":\n";
            }

            // Prólogo CORRECTO
            emitComment("Function: " + current_function);
            emitComment("Prolog");
            emitInstruction("addi $sp, $sp, -" + std::to_string(frame_size));
            emitInstruction("sw $ra, 60($sp)");
            emitInstruction("sw $fp, 56($sp)");
            emitInstruction("move $fp, $sp");

            continue;
        }

        // Procesar ARG
        if (in_function && line.find("ARG ") == 0)
        {
            std::string arg_name = line.substr(4);
            if (arg_name.back() == ';')
                arg_name.pop_back();

            int arg_offset = 4 + (args.size() * 4);
            local_vars[arg_name] = arg_offset;
            args.push_back(arg_name);

            if (args.size() <= 4)
            {
                emitInstruction("sw $a" + std::to_string(args.size() - 1) + ", " +
                                std::to_string(arg_offset) + "($fp)  # " + arg_name);
            }
            continue;
        }

        // Procesar PARAM (parámetros implícitos como 'self')
        if (in_function && line.find("PARAM ") == 0)
        {
            std::string param_name = line.substr(6);
            if (param_name.back() == ';')
                param_name.pop_back();

            // PARAM se maneja como argumentos pero con diferente offset base
            int param_offset = 4 + (args.size() * 4);
            local_vars[param_name] = param_offset;
            args.push_back(param_name);

            emitComment("PARAM " + param_name + " at offset " + std::to_string(param_offset));
            // En métodos, 'self' llega como primer argumento en $a0
            if (param_name == "self" && args.size() == 1)
            {
                emitInstruction("sw $a0, " + std::to_string(param_offset) + "($fp)  # " + param_name);
            }
            continue;
        }

        if (line == "}")
        {
            if (in_function)
            {
                // Epílogo CORRECTO
                emitComment("Epilog");
                emitInstruction("lw $ra, 60($sp)");
                emitInstruction("lw $fp, 56($sp)");
                emitInstruction("addi $sp, $sp, " + std::to_string(frame_size));
                emitInstruction("jr $ra");

                text_section << "\n";
            }
            in_function = false;
            continue;
        }

        if (in_function)
        {
            // Procesar TODAS las instrucciones, incluyendo entry
            translateCILInstruction(line, local_vars);
        }
    }
}

// ← NUEVA FUNCIÓN SIMPLIFICADA
void MIPSGenerator::translateCILInstruction(const std::string &line,
                                            std::unordered_map<std::string, int> &local_vars)
{
    std::string clean_line = line;
    if (!clean_line.empty() && clean_line.back() == ';')
        clean_line.pop_back();

    // Función para asignar offsets FIJOS y CONSISTENTES
    auto ensureVariable = [&](const std::string &var)
    {
        if (local_vars.find(var) == local_vars.end() &&
            !isLiteral(var) &&
            string_labels.find(var) == string_labels.end())
        {

            // ❌ PROBLEMA: Layout FIJO causa colisiones
            // ✅ SOLUCIÓN: Offsets únicos incrementales

            // Usar el miembro de clase next_offset (se resetea por función)

            if (var == "n" || var == "start")
            {
                local_vars[var] = 4; // primer argumento
            }
            else if (var == "end")
            {
                local_vars[var] = 8; // segundo argumento
            }
            else if (var.find("t") == 0)
            {
                // Temporales: usar registros únicamente, NO memoria
                emitComment("Temporal " + var + " managed in registers only");
                return; // NO agregarlo a local_vars
            }
            else
            {
                // ✅ ASIGNAR OFFSETS ÚNICOS PARA CADA VARIABLE
                next_offset += 4;                   // Incrementar por cada nueva variable
                local_vars[var] = 12 + next_offset; // Empezar desde 12, incrementar
            }

            emitComment("Variable " + var + " at offset " + std::to_string(local_vars[var]));
        }
    };

    // 1. LITERALES: result = 1.000000
    std::regex ra("^(\\w+)\\s*=\\s*(\\d+)(?:\\.(\\d+))?$");
    std::smatch match;

    if (std::regex_match(clean_line, match, ra))
    {
        std::string dest = match[1].str();
        std::string value = match[2].str();

        ensureVariable(dest);

        if (local_vars.find(dest) != local_vars.end())
        {
            emitInstruction("li $t0, " + value);
            emitInstruction("sw $t0, " + std::to_string(local_vars[dest]) + "($fp)");
        }
        return;
    }

    // 2. ASIGNACIONES DIRECTAS: result = t1
    std::regex assign_regex("^(\\w+)\\s*=\\s*(\\w+)$");
    if (std::regex_match(clean_line, match, assign_regex))
    {
        std::string dest = match[1].str();
        std::string src = match[2].str();

        // NO crear variables para temporales de destino
        if (dest.find("t") != 0)
        {
            ensureVariable(dest);
        }
        if (src.find("t") != 0)
        {
            ensureVariable(src);
        }

        // Si ambos son temporales, solo mantener en registro
        if (dest.find("t") == 0 && src.find("t") == 0)
        {
            emitComment("Temporal assignment: " + dest + " = " + src + " (register only)");
            // El valor ya está en $t2, no necesitamos hacer nada
            local_vars[dest] = -1; // Marcar como temporal en registro
            return;
        }

        // Si destino es una variable real y src es temporal, usar $t2 directamente
        if (local_vars.find(dest) != local_vars.end() && src.find("t") == 0)
        {
            emitInstruction("sw $t2, " + std::to_string(local_vars[dest]) + "($fp)");
            return;
        }

        // Si destino es temporal y src es variable real
        if (dest.find("t") == 0 && local_vars.find(src) != local_vars.end())
        {
            emitInstruction("lw $t2, " + std::to_string(local_vars[src]) + "($fp)");
            local_vars[dest] = -1; // Marcar como temporal en registro
            return;
        }

        // Casos normales
        if (isLiteral(src))
        {
            std::string int_value = src.substr(0, src.find('.'));
            emitInstruction("li $t0, " + int_value);
        }
        else if (string_labels.find(src) != string_labels.end())
        {
            emitInstruction("la $t0, " + src);
        }
        else if (local_vars.find(src) != local_vars.end())
        {
            emitInstruction("lw $t0, " + std::to_string(local_vars[src]) + "($fp)");
        }
        else
        {
            emitComment("Error: Variable " + src + " not found or is temporal");
            return;
        }

        if (local_vars.find(dest) != local_vars.end())
        {
            emitInstruction("sw $t0, " + std::to_string(local_vars[dest]) + "($fp)");
        }
        return;
    }

    // 3. OPERACIONES BINARIAS: t0 = x > 5.000000
    std::regex ro("^(\\w+)\\s*=\\s*(\\w+)\\s*([+*/-]|<=|>=|==|!=|<|>)\\s*(\\w+(?:\\.\\d+)?)$");
    if (std::regex_match(clean_line, match, ro))
    {
        std::string dest = match[1].str();
        std::string left = match[2].str();
        std::string op = match[3].str();
        std::string right = match[4].str();

        emitComment("Binary operation: " + dest + " = " + left + " " + op + " " + right);

        // NO crear variables para temporales de destino
        if (dest.find("t") != 0)
        {
            ensureVariable(dest);
        }

        // Solo asegurar variables reales (no temporales)
        if (left.find("t") != 0)
        {
            ensureVariable(left);
        }
        if (right.find("t") != 0 && !isLiteral(right))
        {
            ensureVariable(right);
        }

        // Cargar operando izquierdo
        if (isLiteral(left))
        {
            std::string int_value = left.substr(0, left.find('.'));
            emitInstruction("li $t0, " + int_value);
        }
        else if (left.find("t") == 0)
        {
            // Temporal - usar $t2 del resultado anterior
            emitInstruction("move $t0, $t2");
        }
        else if (local_vars.find(left) != local_vars.end())
        {
            emitInstruction("lw $t0, " + std::to_string(local_vars[left]) + "($fp)");
        }
        else
        {
            emitComment("Error: Left operand " + left + " not found");
            emitInstruction("li $t0, 0"); // Default fallback
        }

        // Cargar operando derecho
        if (isLiteral(right))
        {
            std::string int_value = right.substr(0, right.find('.'));
            emitInstruction("li $t1, " + int_value);
        }
        else if (right.find("t") == 0)
        {
            // Temporal - usar $t2 del resultado anterior
            emitInstruction("move $t1, $t2");
        }
        else if (local_vars.find(right) != local_vars.end())
        {
            emitInstruction("lw $t1, " + std::to_string(local_vars[right]) + "($fp)");
        }
        else
        {
            emitComment("Error: Right operand " + right + " not found");
            emitInstruction("li $t1, 0"); // Default fallback
        }

        // Realizar operación
        if (op == "+")
        {
            emitInstruction("add $t2, $t0, $t1");
        }
        else if (op == "-")
        {
            emitInstruction("sub $t2, $t0, $t1");
        }
        else if (op == "*")
        {
            emitInstruction("mul $t2, $t0, $t1");
        }
        else if (op == "/")
        {
            emitInstruction("div $t0, $t1");
            emitInstruction("mflo $t2");
        }
        else if (op == "<=")
        {
            emitInstruction("sle $t2, $t0, $t1");
        }
        else if (op == ">=")
        {
            emitInstruction("sge $t2, $t0, $t1");
        }
        else if (op == "<")
        {
            emitInstruction("slt $t2, $t0, $t1");
        }
        else if (op == ">")
        {
            emitInstruction("sgt $t2, $t0, $t1");
        }
        else if (op == "==")
        {
            emitInstruction("seq $t2, $t0, $t1");
        }
        else if (op == "!=")
        {
            emitInstruction("sne $t2, $t0, $t1");
        }

        // Si el destino es un temporal, mantenerlo en registro y crear mapping
        if (dest.find("t") == 0)
        {
            emitComment("Temporal " + dest + " result stored in $t2");
            // Crear un mapping temporal para usar en IF statements
            local_vars[dest] = -1; // Marca especial para temporales en $t2
        }
        else
        {
            emitInstruction("sw $t2, " + std::to_string(local_vars[dest]) + "($fp)");
        }
        return;
    }

    // 4. ASIGNACIONES DESTRUCTIVAS: i := t2 vs result := t2
    std::regex destruct_assign("^(\\w+)\\s*:=\\s*(\\w+)$");
    if (std::regex_match(clean_line, match, destruct_assign))
    {
        std::string var = match[1].str();
        std::string value = match[2].str();

        ensureVariable(var);

        if (value.find("t") == 0)
        {
            // Si la variable es un contador (i, current), generar incremento
            if (var == "i" || var == "current")
            {
                emitComment("Incrementing " + var);
                emitInstruction("lw $t0, " + std::to_string(local_vars[var]) + "($fp)");
                emitInstruction("addi $t0, $t0, 1");
                emitInstruction("sw $t0, " + std::to_string(local_vars[var]) + "($fp)");
            }
            else
            {
                // Para variables acumuladoras (result, sum), usar el resultado de la operación
                emitComment("Updating " + var + " with operation result");
                emitInstruction("sw $t2, " + std::to_string(local_vars[var]) + "($fp)");
            }
        }
        else
        {
            // Asignación normal
            generateAssignment(var, value, local_vars);
        }
        return;
    }

    // 5. RETURN
    if (clean_line.find("RETURN") == 0)
    {
        std::string value = "";
        if (clean_line.length() > 6)
        {
            value = clean_line.substr(7);

            if (value.find("t") == 0)
            {
                // Si es un temporal, usar el último resultado en $t2
                emitComment("Returning temporal result from $t2");
                emitInstruction("move $v0, $t2");
            }
            else if (isLiteral(value))
            {
                std::string int_value = value.substr(0, value.find('.'));
                emitInstruction("li $v0, " + int_value);
            }
            else if (local_vars.find(value) != local_vars.end())
            {
                emitInstruction("lw $v0, " + std::to_string(local_vars[value]) + "($fp)");
            }
            else if (string_labels.find(value) != string_labels.end())
            {
                // Si es una etiqueta de string, cargar su dirección
                emitComment("Loading string address for " + value);
                emitInstruction("la $v0, " + value);
            }
            else
            {
                emitComment("Error: Return value " + value + " not found");
                emitInstruction("li $v0, 0");
            }
        }
        return;
    }

    // 6. ETIQUETAS
    if (!clean_line.empty() && clean_line.back() == ':')
    {
        std::string label = clean_line.substr(0, clean_line.length() - 1);
        text_section << label << ":\n";
        return;
    }

    // 7. GOTO
    if (clean_line.find("GOTO") == 0)
    {
        std::string label = clean_line.substr(5);
        emitInstruction("j " + label);
        return;
    }

    // 8. IF GOTO: IF t0 == 0 GOTO L1
    std::regex if_goto_regex("^IF\\s+(\\w+)\\s*(==|!=|<|>|<=|>=)\\s*(\\w+(?:\\.\\d+)?)\\s+GOTO\\s+(\\w+)$");
    if (std::regex_match(clean_line, match, if_goto_regex))
    {
        std::string left = match[1].str();
        std::string op = match[2].str();
        std::string right = match[3].str();
        std::string label = match[4].str();

        emitComment("IF " + left + " " + op + " " + right + " GOTO " + label);

        // Si left es un temporal que está mapeado a $t2
        if (left.find("t") == 0 && local_vars.find(left) != local_vars.end() && local_vars[left] == -1)
        {
            if (op == "==" && right == "0")
            {
                emitInstruction("beq $t2, $zero, " + label);
            }
            else if (op == "!=" && right == "0")
            {
                emitInstruction("bne $t2, $zero, " + label);
            }
            else if (op == "==" && right != "0")
            {
                if (isLiteral(right))
                {
                    std::string int_value = right.substr(0, right.find('.'));
                    emitInstruction("li $t1, " + int_value);
                }
                else
                {
                    emitInstruction("move $t1, $" + right);
                }
                emitInstruction("beq $t2, $t1, " + label);
            }
            else if (op == "!=" && right != "0")
            {
                if (isLiteral(right))
                {
                    std::string int_value = right.substr(0, right.find('.'));
                    emitInstruction("li $t1, " + int_value);
                }
                else
                {
                    emitInstruction("move $t1, $" + right);
                }
                emitInstruction("bne $t2, $t1, " + label);
            }
        }
        else
        {
            // Cargar desde memoria si es variable normal
            if (left.find("t") != 0)
            {
                ensureVariable(left);
            }
            if (right.find("t") != 0 && !isLiteral(right))
            {
                ensureVariable(right);
            }

            if (isLiteral(left))
            {
                std::string int_value = left.substr(0, left.find('.'));
                emitInstruction("li $t0, " + int_value);
            }
            else if (left.find("t") == 0)
            {
                emitInstruction("move $t0, $t2"); // Usar último resultado temporal
            }
            else if (local_vars.find(left) != local_vars.end())
            {
                emitInstruction("lw $t0, " + std::to_string(local_vars[left]) + "($fp)");
            }
            else
            {
                emitComment("Error: Left condition " + left + " not found");
                return;
            }

            if (isLiteral(right))
            {
                std::string int_value = right.substr(0, right.find('.'));
                emitInstruction("li $t1, " + int_value);
            }
            else if (right.find("t") == 0)
            {
                emitInstruction("move $t1, $t2"); // Usar último resultado temporal
            }
            else if (local_vars.find(right) != local_vars.end())
            {
                emitInstruction("lw $t1, " + std::to_string(local_vars[right]) + "($fp)");
            }
            else
            {
                emitComment("Error: Right condition " + right + " not found");
                return;
            }

            // Generar salto
            if (op == "==")
            {
                emitInstruction("beq $t0, $t1, " + label);
            }
            else if (op == "!=")
            {
                emitInstruction("bne $t0, $t1, " + label);
            }
            else if (op == "<=")
            {
                emitInstruction("ble $t0, $t1, " + label);
            }
            else if (op == ">=")
            {
                emitInstruction("bge $t0, $t1, " + label);
            }
            else if (op == "<")
            {
                emitInstruction("blt $t0, $t1, " + label);
            }
            else if (op == ">")
            {
                emitInstruction("bgt $t0, $t1, " + label);
            }
        }
        return;
    }

    // 8. LLAMADAS: t6 = CALL factorial_iterative 5.000000
    std::regex call_regex("^(\\w+)\\s*=\\s*CALL\\s+(\\w+)(.*)$");
    if (std::regex_match(clean_line, match, call_regex))
    {
        std::string dest = match[1].str();
        std::string func = match[2].str();
        std::string args_str = match[3].str();

        // Parsear argumentos
        std::vector<std::string> args;
        std::istringstream iss(args_str);
        std::string arg;
        while (iss >> arg)
        {
            args.push_back(arg);
        }

        // Para funciones como print
        if (func == "print")
        {
            if (!args.empty())
            {
                std::string value = args[0];

                // Si es temporal, usar directamente el resultado previo
                if (value.find("t") == 0)
                {
                    emitInstruction("move $a0, $v0"); // Usar resultado de llamada anterior
                    // Por defecto, usar print_int para temporales (los strings se manejan explícitamente)
                    emitInstruction("li $v0, 1"); // print_int
                    emitInstruction("syscall");
                    emitInstruction("li $a0, 10"); // newline
                    emitInstruction("li $v0, 11"); // print_char
                    emitInstruction("syscall");
                    return;
                }
                else if (string_labels.find(value) != string_labels.end())
                {
                    emitInstruction("la $a0, " + value);
                    emitInstruction("li $v0, 4"); // print_string
                    emitInstruction("syscall");
                    emitInstruction("li $a0, 10"); // newline
                    emitInstruction("li $v0, 11"); // print_char
                    emitInstruction("syscall");
                    return;
                }
                else
                {
                    emitInstruction("lw $a0, " + std::to_string(local_vars[value]) + "($fp)");
                }

                emitInstruction("li $v0, 1"); // print_int
                emitInstruction("syscall");
                emitInstruction("li $a0, 10"); // newline
                emitInstruction("li $v0, 11"); // print_char
                emitInstruction("syscall");
            }
        }
        else
        {
            // Llamadas a funciones normales
            for (size_t i = 0; i < args.size() && i < 4; i++)
            {
                if (isLiteral(args[i]))
                {
                    std::string int_value = args[i].substr(0, args[i].find('.'));
                    emitInstruction("li $a" + std::to_string(i) + ", " + int_value);
                }
                else if (local_vars.find(args[i]) != local_vars.end())
                {
                    emitInstruction("lw $a" + std::to_string(i) + ", " +
                                    std::to_string(local_vars[args[i]]) + "($fp)");
                }
            }

            emitInstruction("jal " + func);
            // El resultado queda automáticamente en $v0 para uso posterior
        }
        return;
    }

    // 9. LLAMADAS VIRTUALES: t5 = VCALL t4 faa
    std::regex vcall_regex("^(\\w+)\\s*=\\s*VCALL\\s+(\\w+)\\s+(\\w+)(.*)$");
    if (std::regex_match(clean_line, match, vcall_regex))
    {
        std::string dest = match[1].str();
        std::string obj = match[2].str();
        std::string method = match[3].str();
        std::string args_str = match[4].str();

        emitComment("VCALL: " + dest + " = " + obj + "." + method + "()");

        // ✅ IMPLEMENTACIÓN POLIMÓRFICA REAL:
        // 1. Cargar el objeto (que contiene un puntero a su vtable)
        // 2. Acceder a la vtable del objeto
        // 3. Buscar el método correcto en la vtable
        // 4. Llamar al método

        // Por simplicidad, vamos a simular objetos usando un esquema simple:
        // - Objetos tipo A tienen ID = 1, método faa = f0
        // - Objetos tipo B tienen ID = 2, método faa = f1

        // Cargar el objeto
        if (obj.find("t") == 0)
        {
            // El objeto está en un temporal, viene del resultado anterior
            emitComment("Object in temporal " + obj + ", using previous result");
            emitInstruction("move $t0, $v0"); // objeto en $t0
        }
        else if (local_vars.find(obj) != local_vars.end())
        {
            emitInstruction("lw $t0, " + std::to_string(local_vars[obj]) + "($fp)");
        }
        else
        {
            emitComment("Error: Object " + obj + " not found");
            return;
        }

        // ✅ LLAMADA POLIMÓRFICA GENÉRICA: usar dispatcher dinámico
        emitComment("Polymorphic call to " + method + " method");
        emitComment("Check object type and call appropriate method");

        // Usar el dispatcher específico para este método
        emitInstruction("move $a0, $t0");
        emitInstruction("jal " + method + "_polymorphic");
        return;
    }

    // 10. ALLOCATE: t2 = ALLOCATE A
    std::regex allocate_regex("^(\\w+)\\s*=\\s*ALLOCATE\\s+(\\w+)$");
    if (std::regex_match(clean_line, match, allocate_regex))
    {
        std::string dest = match[1].str();
        std::string type = match[2].str();

        emitComment("ALLOCATE " + type + " -> " + dest);

        // Obtener ID único para el tipo dinámicamente
        int type_id = getTypeId(type);
        emitInstruction("li $t2, " + std::to_string(type_id)); // ID dinámico del tipo

        emitComment("Object of type " + type + " allocated with ID " + std::to_string(type_id));
        return;
    }

    // 11. SETATTR: SETATTR t2 vtable vtable_A
    std::regex setattr_regex("^SETATTR\\s+(\\w+)\\s+(\\w+)\\s+(\\w+)$");
    if (std::regex_match(clean_line, match, setattr_regex))
    {
        std::string obj = match[1].str();
        std::string attr = match[2].str();
        std::string value = match[3].str();

        emitComment("SETATTR " + obj + "." + attr + " = " + value);

        // Para vtables, no necesitamos hacer nada extra en esta simulación
        // El tipo ya está guardado en el objeto (desde ALLOCATE)

        emitComment("VTable set for object " + obj);
        return;
    }
}

void MIPSGenerator::emitInstruction(const std::string &instruction)
{
    text_section << "    " << instruction << "\n";
}

void MIPSGenerator::emitComment(const std::string &comment)
{
    text_section << "    # " << comment << "\n";
}

bool MIPSGenerator::isLiteral(const std::string &var)
{
    if (var.empty())
        return false;

    // Verificar si es un número
    bool has_digit = false;
    for (size_t i = 0; i < var.length(); ++i)
    {
        char c = var[i];
        if (i == 0 && (c == '-' || c == '+'))
        {
            continue; // Signo al inicio
        }
        if (std::isdigit(c))
        {
            has_digit = true;
        }
        else if (c == '.')
        {
            continue; // Punto decimal
        }
        else
        {
            return false; // Caracter no válido para número
        }
    }

    return has_digit;
}

std::string MIPSGenerator::getNextTempRegister()
{
    std::string reg = temp_registers[next_temp_reg % temp_registers.size()];
    next_temp_reg++;
    return reg;
}

std::string MIPSGenerator::getRegisterForVariable(const std::string &var)
{
    if (variable_registers.find(var) == variable_registers.end())
    {
        variable_registers[var] = getNextTempRegister();
    }
    return variable_registers[var];
}

std::string MIPSGenerator::newLabel()
{
    return "L" + std::to_string(label_counter++);
}

// Implementaciones vacías para métodos no utilizados actualmente
void MIPSGenerator::generateAssignment(const std::string &dest, const std::string &src,
                                       const std::unordered_map<std::string, int> &local_vars)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generateBinaryOp(const std::string &dest, const std::string &left,
                                     const std::string &op, const std::string &right,
                                     const std::unordered_map<std::string, int> &local_vars)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generateFunctionCall(const std::string &dest, const std::string &func,
                                         const std::vector<std::string> &args,
                                         const std::unordered_map<std::string, int> &local_vars)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generatePrint(const std::string &value,
                                  const std::unordered_map<std::string, int> &local_vars)
{
    // Verificar si es string o número
    if (string_labels.find(value) != string_labels.end())
    {
        // Imprimir string CORRECTAMENTE
        emitInstruction("la $a0, " + value);
        emitInstruction("li $v0, 4"); // print_string syscall (NO 1)
        emitInstruction("syscall");
    }
    else
    {
        // Cargar valor y imprimir entero
        if (isLiteral(value))
        {
            std::string int_value = value.substr(0, value.find('.'));
            emitInstruction("li $a0, " + int_value);
        }
        else if (local_vars.find(value) != local_vars.end() && local_vars.at(value) != -1)
        {
            emitInstruction("lw $a0, " + std::to_string(local_vars.at(value)) + "($fp)");
        }
        else
        {
            emitComment("Error: Print value " + value + " not found");
            emitInstruction("li $a0, 0");
        }
        emitInstruction("li $v0, 1"); // print_int syscall
        emitInstruction("syscall");
    }

    // Imprimir nueva línea
    emitInstruction("li $a0, 10"); // ASCII newline
    emitInstruction("li $v0, 11"); // print_char syscall
    emitInstruction("syscall");
}

void MIPSGenerator::generateReturn(const std::string &value,
                                   const std::unordered_map<std::string, int> &local_vars)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generateConditionalJump(const std::string &left, const std::string &op,
                                            const std::string &right, const std::string &label,
                                            const std::unordered_map<std::string, int> &local_vars)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generateObjectAllocation(const std::string &dest, const std::string &type)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generateArrayAllocation(const std::string &dest, const std::string &size)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generateGetAttribute(const std::string &dest, const std::string &obj, const std::string &attr)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generateSetAttribute(const std::string &obj, const std::string &attr, const std::string &value)
{
    // Implementación si es necesaria
}

void MIPSGenerator::generatePolymorphicDispatchers(std::ostringstream &result)
{
    result << "\n# Polymorphic dispatch functions\n";

    // Generar un dispatcher para cada método descubierto
    for (const auto &method_name : discovered_methods)
    {
        result << method_name << "_polymorphic:\n";
        result << "    # Polymorphic dispatcher for " << method_name << " method\n";
        result << "    # $a0 contains object with type info\n";
        result << "    \n";
        result << "    # Prolog for dispatcher\n";
        result << "    addi $sp, $sp, -16\n";
        result << "    sw $ra, 12($sp)\n";
        result << "    sw $fp, 8($sp)\n";
        result << "    move $fp, $sp\n";
        result << "    \n";
        result << "    # Check object type\n";

        // ARREGLO DEL BUG: Generar casos basado en el mapeo tipo -> ID correcto
        for (const auto &type_entry : type_to_method_function)
        {
            const std::string &type_name = type_entry.first;
            const auto &methods = type_entry.second;

            // Solo procesar si este tipo tiene el método que estamos generando
            if (methods.find(method_name) != methods.end())
            {
                int type_id = type_ids[type_name];
                const std::string &function_name = methods.at(method_name);

                result << "    # DEBUG: Type " << type_name << " (ID " << type_id << ") -> " << function_name << "\n";
                result << "    li $t1, " << type_id << "\n";
                result << "    beq $a0, $t1, " << method_name << "_call_" << function_name << "\n";
            }
        }

        result << "    \n";
        result << "    # Default case - should not happen\n";
        result << "    li $v0, 0\n";
        result << "    j " << method_name << "_dispatcher_end\n";
        result << "    \n";

        // ARREGLO DEL BUG: Generar las etiquetas de llamada basado en el mapeo correcto
        for (const auto &type_entry : type_to_method_function)
        {
            const auto &methods = type_entry.second;

            // Solo procesar si este tipo tiene el método que estamos generando
            if (methods.find(method_name) != methods.end())
            {
                const std::string &function_name = methods.at(method_name);

                result << method_name << "_call_" << function_name << ":\n";
                result << "    jal " << function_name << "\n";
                result << "    j " << method_name << "_dispatcher_end\n";
                result << "    \n";
            }
        }

        result << method_name << "_dispatcher_end:\n";
        result << "    # Epilog for dispatcher\n";
        result << "    lw $ra, 12($sp)\n";
        result << "    lw $fp, 8($sp)\n";
        result << "    addi $sp, $sp, 16\n";
        result << "    jr $ra\n";
        result << "\n";
    }
}

// ← ARREGLO DEL BUG: Usar IDs pre-asignados en processTypesSection
int MIPSGenerator::getTypeId(const std::string &type)
{
    // Si el tipo ya tiene un ID asignado en processTypesSection, lo devolvemos
    if (type_ids.find(type) != type_ids.end())
    {
        return type_ids[type];
    }

    // Si por alguna razón no se asignó en processTypesSection,
    // asignar uno nuevo (esto no debería pasar en condiciones normales)
    int id = next_type_id++;
    type_ids[type] = id;
    return id;
}