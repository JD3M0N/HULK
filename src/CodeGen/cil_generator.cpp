#include "cil_generator.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

CILGenerator::CILGenerator()
{
    // Inicializar secciones en el orden correcto
    types_section << ".TYPES\n";
    data_section << ".DATA\n";
    code_section << ".CODE\n";

    // Inicializar contador de IDs de métodos
    method_id_counter = 0;
}

std::string CILGenerator::generateCode(Program *program)
{
    // Generar código para el programa
    program->accept(this);

    // Construir el resultado final CON SECCIÓN DE TIPOS
    std::ostringstream result;
    result << types_section.str() << "\n"; // ← TIPOS PRIMERO
    result << data_section.str() << "\n";
    result << code_section.str();

    return result.str();
}

std::string CILGenerator::newTemp()
{
    return "t" + std::to_string(temp_counter++);
}

std::string CILGenerator::newLabel()
{
    return "L" + std::to_string(label_counter++);
}

std::string CILGenerator::newFunctionLabel()
{
    return "f" + std::to_string(function_counter++);
}

std::string CILGenerator::getStringLabel(const std::string &str)
{
    if (string_labels.find(str) == string_labels.end())
    {
        std::string label = "str" + std::to_string(string_constants.size());
        string_labels[str] = label;
        string_constants.push_back(str);

        // Agregar a la sección de datos
        data_section << label << " = \"" << str << "\";\n";
    }
    return string_labels[str];
}

void CILGenerator::emitInstruction(const std::string &instruction)
{
    code_section << "    " << instruction << ";\n";
}

void CILGenerator::emitAssignment(const std::string &dest, const std::string &source)
{
    emitInstruction(dest + " = " + source);
}

void CILGenerator::emitBinaryOp(const std::string &dest, const std::string &left,
                                const std::string &op, const std::string &right)
{
    emitInstruction(dest + " = " + left + " " + op + " " + right);
}

void CILGenerator::emitTypeDeclaration(const std::string &type_name,
                                       const std::vector<std::string> &attributes,
                                       const std::vector<std::pair<std::string, std::string>> &methods)
{
    types_section << "type " << type_name << " {\n";

    // Emitir atributos
    for (const auto &attr : attributes)
    {
        types_section << "    attribute " << type_name << "_" << attr << " ;\n";
    }

    // Emitir métodos
    for (const auto &method : methods)
    {
        types_section << "    method " << type_name << "_" << method.first << ": "
                      << method.second << " ;\n";
    }

    types_section << "}\n";
}

// Nuevo método para manejar herencia según especificación

void CILGenerator::emitTypeDeclarationWithInheritance(
    const std::string &type_name,
    const std::string &parent_name,
    const std::vector<std::string> &attributes,
    const std::vector<std::pair<std::string, std::string>> &methods)
{
    types_section << "type " << type_name;

    // AGREGAR INFORMACIÓN DE HERENCIA si no es Object
    if (parent_name != "Object")
    {
        types_section << " inherits " << parent_name;
    }

    types_section << " {\n";

    // Emitir atributos CON PREFIJO DE TIPO (según especificación)
    for (const auto &attr : attributes)
    {
        types_section << "    attribute " << type_name << "_" << attr << " ;\n";
    }

    // Emitir métodos CON PREFIJO DE TIPO
    for (const auto &method : methods)
    {
        types_section << "    method " << type_name << "_" << method.first << ": "
                      << method.second << " ;\n";
    }

    types_section << "}\n";
}

void CILGenerator::visit(Program *prog)
{
    // Solo procesar como programa principal si current_function está vacío
    if (current_function.empty())
    {
        // NUEVO: Primero registrar todas las clases para resolver herencia
        for (auto &stmt : prog->stmts)
        {
            auto *cls = dynamic_cast<ClassDecl *>(stmt.get());
            if (cls)
            {
                registerClass(cls);
            }
        }

        // Construir VTables para todas las clases
        for (auto &stmt : prog->stmts)
        {
            auto *cls = dynamic_cast<ClassDecl *>(stmt.get());
            if (cls)
            {
                buildVTable(cls);
            }
        }

        // Luego generar todas las clases (que incluyen los tipos)
        for (auto &stmt : prog->stmts)
        {
            auto *cls = dynamic_cast<ClassDecl *>(stmt.get());
            if (cls)
            {
                cls->accept(this);
            }
        }

        // Luego generar todas las funciones
        for (auto &stmt : prog->stmts)
        {
            auto *func = dynamic_cast<FunctionDecl *>(stmt.get());
            if (func)
            {
                func->accept(this);
            }
        }

        // Crear función entry para el código restante (según especificación del libro)
        bool hasMainCode = false;
        for (auto &stmt : prog->stmts)
        {
            auto *func = dynamic_cast<FunctionDecl *>(stmt.get());
            auto *cls = dynamic_cast<ClassDecl *>(stmt.get());
            if (!func && !cls)
            {
                hasMainCode = true;
                break;
            }
        }

        if (hasMainCode)
        {
            code_section << "\nfunction entry {\n";
            current_function = "entry";

            // Procesar statements del main
            for (auto &stmt : prog->stmts)
            {
                auto *func = dynamic_cast<FunctionDecl *>(stmt.get());
                auto *cls = dynamic_cast<ClassDecl *>(stmt.get());
                if (!func && !cls)
                {
                    stmt->accept(this);
                }
            }

            emitInstruction("RETURN 0");
            code_section << "}\n";
        }
    }
    else
    {
        // Estamos dentro de una función - solo procesar statements
        for (auto &stmt : prog->stmts)
        {
            stmt->accept(this);
        }
    }
}

void CILGenerator::visit(ExprStmt *stmt)
{
    stmt->expr->accept(this);
    // El resultado está en last_temp, pero para ExprStmt no necesitamos hacer nada más
}

void CILGenerator::visit(FunctionDecl *func)
{
    current_function = func->name;
    function_locals.clear();
    function_args.clear();

    code_section << "\nfunction " << func->name << " {\n";

    // Declarar argumentos
    for (const auto &param : func->params)
    {
        code_section << "    ARG " << param << ";\n";
        function_args.push_back(param);
    }

    // Generar código del cuerpo
    func->body->accept(this);

    // Si la función devuelve algo, usar RETURN
    if (!last_temp.empty())
    {
        emitInstruction("RETURN " + last_temp);
    }

    code_section << "}\n";
}

void CILGenerator::visit(ClassDecl *cls)
{
    // 1. Generar declaración de tipo con herencia correcta
    std::vector<std::string> attr_names;
    std::vector<std::pair<std::string, std::string>> method_info;

    // NUEVO: Si hay herencia, primero agregar atributos del padre
    if (cls->parent != "Object" && !cls->parent.empty())
    {
        // Buscar la clase padre en el AST del programa actual
        ClassDecl *parent_class = findParentClass(cls->parent);
        if (parent_class)
        {
            // Agregar atributos del padre EN EL MISMO ORDEN
            for (const auto &parent_attr : parent_class->attributes)
            {
                attr_names.push_back(parent_attr.first);
            }

            // Recursivamente agregar atributos de ancestros más lejanos
            addAncestorAttributes(parent_class, attr_names);
        }
        else
        {
            // Si no se encuentra la clase padre, agregar placeholder
            std::cerr << "Advertencia: Clase padre '" << cls->parent << "' no encontrada para '" << cls->name << "'" << std::endl;
        }
    }

    // Agregar atributos de la clase actual (manteniendo orden)
    for (const auto &attr : cls->attributes)
    {
        // Verificar que no sea una redefinición de atributo heredado
        if (std::find(attr_names.begin(), attr_names.end(), attr.first) == attr_names.end())
        {
            attr_names.push_back(attr.first);
        }
        else
        {
            std::cerr << "Advertencia: Atributo '" << attr.first << "' redefinido en clase '" << cls->name << "'" << std::endl;
        }
    }

    // Usar VTable para obtener información de métodos
    if (vtables.find(cls->name) != vtables.end())
    {
        const VTable &vtable = vtables[cls->name];
        for (const auto &entry : vtable.entries)
        {
            method_info.push_back({entry.method_name, entry.function_label});
        }
    }

    // Emitir declaración de tipo CON INFORMACIÓN DE HERENCIA
    emitTypeDeclarationWithInheritance(cls->name, cls->parent, attr_names, method_info);

    // Emitir VTable en la sección de datos
    if (vtables.find(cls->name) != vtables.end())
    {
        emitVTableDeclaration(cls->name, vtables[cls->name]);
    }

    // 2. Generar métodos usando la VTable
    if (vtables.find(cls->name) != vtables.end())
    {
        const VTable &vtable = vtables[cls->name];

        // Generar métodos de la clase actual
        for (auto &method : cls->methods)
        {
            auto *func = dynamic_cast<FunctionDecl *>(method.get());
            if (func)
            {
                // Encontrar la etiqueta correspondiente en la VTable
                std::string method_label;
                auto it = vtable.method_to_index.find(func->name);
                if (it != vtable.method_to_index.end())
                {
                    method_label = vtable.entries[it->second].function_label;
                }
                else
                {
                    method_label = newFunctionLabel();
                }

                code_section << "\nfunction " << method_label << " {\n";
                current_function = method_label;

                // Agregar 'self' como primer parámetro
                code_section << "    PARAM self;\n";

                // Agregar parámetros del método
                for (const auto &param : func->params)
                {
                    code_section << "    ARG " << param << ";\n";
                }

                // Generar código del cuerpo
                func->body->accept(this);

                // Return
                if (!last_temp.empty())
                {
                    emitInstruction("RETURN " + last_temp);
                }
                else
                {
                    emitInstruction("RETURN self"); // Retornar self por defecto
                }

                code_section << "}\n";
            }
        }
    }
}

// ============ ExprVisitor ============
// (El resto de métodos se mantienen igual)

void CILGenerator::visit(NumberExpr *expr)
{
    last_temp = std::to_string(expr->value);
}

void CILGenerator::visit(StringExpr *expr)
{
    last_temp = getStringLabel(expr->value);
}

void CILGenerator::visit(BooleanExpr *expr)
{
    last_temp = expr->value ? "1" : "0";
}

void CILGenerator::visit(UnaryExpr *expr)
{
    expr->operand->accept(this);
    std::string operand = last_temp;

    last_temp = newTemp();

    switch (expr->op)
    {
    case UnaryExpr::OP_NEG:
        emitBinaryOp(last_temp, "0", "-", operand);
        break;
    }
}

void CILGenerator::visit(BinaryExpr *expr)
{
    expr->left->accept(this);
    std::string left = last_temp;

    expr->right->accept(this);
    std::string right = last_temp;

    last_temp = newTemp();

    std::string op;
    switch (expr->op)
    {
    case BinaryExpr::OP_ADD:
        op = "+";
        break;
    case BinaryExpr::OP_SUB:
        op = "-";
        break;
    case BinaryExpr::OP_MUL:
        op = "*";
        break;
    case BinaryExpr::OP_DIV:
        op = "/";
        break;
    case BinaryExpr::OP_MOD:
        op = "%";
        break;
    case BinaryExpr::OP_POW:
        op = "^";
        break;
    case BinaryExpr::OP_LT:
        op = "<";
        break;
    case BinaryExpr::OP_GT:
        op = ">";
        break;
    case BinaryExpr::OP_LE:
        op = "<=";
        break;
    case BinaryExpr::OP_GE:
        op = ">=";
        break;
    case BinaryExpr::OP_EQ:
        op = "==";
        break;
    case BinaryExpr::OP_NEQ:
        op = "!=";
        break;
    case BinaryExpr::OP_AND:
        op = "&&";
        break;
    case BinaryExpr::OP_OR:
        op = "||";
        break;
    case BinaryExpr::OP_CONCAT:
        op = "@";
        break;
    }

    emitBinaryOp(last_temp, left, op, right);
}

void CILGenerator::visit(CallExpr *expr)
{
    // Evaluar argumentos
    std::vector<std::string> args;
    for (auto &arg : expr->args)
    {
        arg->accept(this);
        args.push_back(last_temp);
    }

    last_temp = newTemp();

    // Casos especiales para funciones built-in
    if (expr->callee == "print")
    {
        // print siempre es una función, no un método
        std::string call_instr = last_temp + " = CALL " + expr->callee;
        for (const auto &arg : args)
        {
            call_instr += " " + arg;
        }
        emitInstruction(call_instr);
        return;
    }

    // Verificar si es una llamada a método (el primer argumento es un objeto)
    // Esto se determina por si tenemos más de 0 argumentos y el parser insertó el objeto como primer arg
    if (!args.empty() && expr->args.size() > 0)
    {
        // Intentar determinar si es una llamada a método
        // Si el primer argumento viene de una expresión que no es una simple variable o número,
        // probablemente es una llamada a método
        bool isMethodCall = false;

        // Verificamos si el primer argumento es resultado de una expresión compleja
        if (auto *firstArg = expr->args[0].get())
        {
            // Si no es una simple variable ni número, probablemente es un objeto
            if (!dynamic_cast<VariableExpr *>(firstArg) &&
                !dynamic_cast<NumberExpr *>(firstArg) &&
                !dynamic_cast<StringExpr *>(firstArg) &&
                !dynamic_cast<BooleanExpr *>(firstArg))
            {
                isMethodCall = true;
            }
        }

        if (isMethodCall)
        {
            // Es una llamada a método: obj.method(args)
            std::string obj_temp = args[0]; // El objeto es el primer argumento

            // Obtener los argumentos del método (sin el objeto)
            std::vector<std::string> method_args(args.begin() + 1, args.end());

            // Generar llamada polimórfica usando VCALL
            std::string call_instr = last_temp + " = VCALL " + obj_temp + " " + expr->callee;
            for (const auto &arg : method_args)
            {
                call_instr += " " + arg;
            }

            emitInstruction(call_instr);
            return;
        }
    }

    // Llamada a función regular
    std::string call_instr = last_temp + " = CALL " + expr->callee;
    for (const auto &arg : args)
    {
        call_instr += " " + arg;
    }

    emitInstruction(call_instr);
}

void CILGenerator::visit(VariableExpr *expr)
{
    last_temp = expr->name;
}

void CILGenerator::visit(LetExpr *expr)
{
    // Generar código según la plantilla del libro:
    // <init.code>
    // <var> = <init.value> ;
    // <body.code>
    // <value> = <body.value> ;

    // 1. Generar código del inicializador
    expr->initializer->accept(this);
    std::string init_value = last_temp;

    // 2. Asignar a la variable local
    emitAssignment(expr->name, init_value);

    // 3. Generar código del cuerpo
    expr->body->accept(this);

    // 4. El resultado del let es el resultado del cuerpo (ya está en last_temp)
    // No necesitamos hacer nada más porque last_temp ya contiene el valor correcto
}

void CILGenerator::visit(AssignExpr *expr)
{
    expr->value->accept(this);
    std::string value = last_temp;

    // Usar asignación destructiva para AssignExpr (operador :=)
    emitInstruction(expr->name + " := " + value);
    last_temp = expr->name; // El resultado de la asignación es el valor asignado
}

void CILGenerator::visit(IfExpr *expr)
{
    expr->condition->accept(this);
    std::string condition = last_temp;

    std::string else_label = newLabel();
    std::string end_label = newLabel();
    std::string result_temp = newTemp();

    // Si la condición es falsa, saltar al else
    emitInstruction("IF " + condition + " == 0 GOTO " + else_label);

    // Generar rama then
    expr->thenBranch->accept(this);
    emitAssignment(result_temp, last_temp);
    emitInstruction("GOTO " + end_label);

    // Generar rama else
    code_section << else_label << ":\n";
    expr->elseBranch->accept(this);
    emitAssignment(result_temp, last_temp);

    code_section << end_label << ":\n";

    last_temp = result_temp;
}

void CILGenerator::visit(WhileExpr *expr)
{
    std::string loop_start = newLabel();
    std::string loop_end = newLabel();

    code_section << loop_start << ":\n";

    // Evaluar condición
    expr->condition->accept(this);
    std::string condition = last_temp;

    // Si la condición es falsa, salir del bucle
    emitInstruction("IF " + condition + " == 0 GOTO " + loop_end);

    // Cuerpo del bucle
    expr->body->accept(this);

    // Volver al inicio
    emitInstruction("GOTO " + loop_start);

    code_section << loop_end << ":\n";
}

void CILGenerator::visit(NewExpr *expr)
{
    // Evaluar argumentos del constructor
    std::vector<std::string> args;
    for (auto &arg : expr->args)
    {
        arg->accept(this);
        args.push_back(last_temp);
    }

    last_temp = newTemp();

    // Crear nueva instancia según especificación
    emitInstruction(last_temp + " = ALLOCATE " + expr->typeName);

    // Inicializar la VTable del objeto
    emitInstruction("SETATTR " + last_temp + " vtable vtable_" + expr->typeName);

    // Inicializar atributos con valores por defecto si hay argumentos
    if (!args.empty())
    {
        // Llamar al constructor usando VCALL
        for (const auto &arg : args)
        {
            emitInstruction("PARAM " + arg);
        }
        emitInstruction("PARAM " + last_temp); // self como primer parámetro
        std::string result_temp = newTemp();
        emitInstruction(result_temp + " = VCALL " + expr->typeName + " init");
    }
}

void CILGenerator::visit(SelfExpr *expr)
{
    last_temp = "self";
}

void CILGenerator::visit(BaseExpr *expr)
{
    last_temp = "base";
}

void CILGenerator::visit(MemberAccessExpr *expr)
{
    expr->object->accept(this);
    std::string object = last_temp;

    last_temp = newTemp();
    emitInstruction(last_temp + " = GETATTR " + object + " " + expr->member);
}

void CILGenerator::visit(MemberAssignExpr *expr)
{
    expr->object->accept(this);
    std::string object = last_temp;

    expr->value->accept(this);
    std::string value = last_temp;

    emitInstruction("SETATTR " + object + " " + expr->member + " " + value);
    last_temp = value; // El resultado de la asignación es el valor asignado
}

void CILGenerator::visit(ExprBlock *expr)
{
    // Generar código para cada statement en el bloque
    for (auto &stmt : expr->stmts)
    {
        stmt->accept(this);
    }
    // El resultado del bloque es el resultado de la última expresión
    // que ya está almacenado en last_temp
}

// Implementaciones de métodos auxiliares para herencia

ClassDecl *CILGenerator::findParentClass(const std::string &parent_name)
{
    for (auto *cls : class_registry)
    {
        if (cls && cls->name == parent_name)
        {
            return cls;
        }
    }
    return nullptr;
}

void CILGenerator::addAncestorAttributes(ClassDecl *parent_class, std::vector<std::string> &attr_names)
{
    if (!parent_class || parent_class->parent == "Object")
        return;

    // Buscar el abuelo
    ClassDecl *grandparent = findParentClass(parent_class->parent);
    if (grandparent)
    {
        // Primero agregar atributos del abuelo (orden bottom-up)
        addAncestorAttributes(grandparent, attr_names);

        // Luego agregar atributos del abuelo
        for (const auto &ancestor_attr : grandparent->attributes)
        {
            if (std::find(attr_names.begin(), attr_names.end(), ancestor_attr.first) == attr_names.end())
            {
                // Insertar al principio para mantener orden de herencia
                attr_names.insert(attr_names.begin(), ancestor_attr.first);
            }
        }
    }
}

void CILGenerator::addAncestorMethods(ClassDecl *parent_class, std::vector<std::pair<std::string, std::string>> &method_info)
{
    if (!parent_class || parent_class->parent == "Object")
        return;

    // Buscar el abuelo
    ClassDecl *grandparent = findParentClass(parent_class->parent);
    if (grandparent)
    {
        // Primero agregar métodos del abuelo (orden bottom-up)
        addAncestorMethods(grandparent, method_info);

        // Luego agregar métodos del abuelo
        for (const auto &ancestor_method : grandparent->methods)
        {
            auto *ancestor_func = dynamic_cast<FunctionDecl *>(ancestor_method.get());
            if (ancestor_func)
            {
                // Verificar si ya existe (no agregar duplicados)
                bool exists = false;
                for (const auto &existing : method_info)
                {
                    if (existing.first == ancestor_func->name)
                    {
                        exists = true;
                        break;
                    }
                }

                if (!exists)
                {
                    std::string function_label = newFunctionLabel();
                    // Insertar al principio para mantener orden de herencia
                    method_info.insert(method_info.begin(), {ancestor_func->name, function_label});
                }
            }
        }
    }
}

void CILGenerator::generateInheritedMethods(ClassDecl *parent_class, ClassDecl *current_class,
                                            const std::vector<std::pair<std::string, std::string>> &method_info,
                                            int &method_index)
{
    if (!parent_class)
        return;

    // Generar métodos del padre que NO han sido sobrescritos
    for (auto &parent_method : parent_class->methods)
    {
        auto *parent_func = dynamic_cast<FunctionDecl *>(parent_method.get());
        if (!parent_func)
            continue;

        // Verificar si este método fue sobrescrito en la clase actual
        bool is_overridden = false;
        for (auto &current_method : current_class->methods)
        {
            auto *current_func = dynamic_cast<FunctionDecl *>(current_method.get());
            if (current_func && current_func->name == parent_func->name)
            {
                is_overridden = true;
                break;
            }
        }

        // Si no fue sobrescrito, generar el método heredado
        if (!is_overridden)
        {
            // Encontrar la etiqueta correspondiente
            std::string method_label;
            for (const auto &info : method_info)
            {
                if (info.first == parent_func->name)
                {
                    method_label = info.second;
                    break;
                }
            }

            if (!method_label.empty())
            {
                code_section << "\nfunction " << method_label << " {\n";
                current_function = method_label;

                // Agregar 'self' como primer parámetro
                code_section << "    PARAM self;\n";

                // Agregar parámetros del método
                for (const auto &param : parent_func->params)
                {
                    code_section << "    ARG " << param << ";\n";
                }

                // Generar código del cuerpo del método padre
                parent_func->body->accept(this);

                // Return
                if (!last_temp.empty())
                {
                    emitInstruction("RETURN " + last_temp);
                }
                else
                {
                    emitInstruction("RETURN self");
                }

                code_section << "}\n";
                method_index++;
            }
        }
    }
}

void CILGenerator::registerClass(ClassDecl *cls)
{
    class_registry.push_back(cls);
}

int CILGenerator::newMethodId()
{
    return method_id_counter++;
}

void CILGenerator::buildVTable(ClassDecl *cls)
{
    VTable vtable;
    vtable.class_name = cls->name;

    // Si hay herencia, copiar VTable del padre primero
    if (cls->parent != "Object" && !cls->parent.empty())
    {
        ClassDecl *parent_class = findParentClass(cls->parent);
        if (parent_class && vtables.find(cls->parent) != vtables.end())
        {
            // Copiar entradas del padre
            VTable &parent_vtable = vtables[cls->parent];
            vtable.entries = parent_vtable.entries;
            vtable.method_to_index = parent_vtable.method_to_index;
        }
    }

    // Procesar métodos de la clase actual
    for (auto &method_stmt : cls->methods)
    {
        auto *func = dynamic_cast<FunctionDecl *>(method_stmt.get());
        if (func)
        {
            auto it = vtable.method_to_index.find(func->name);
            if (it != vtable.method_to_index.end())
            {
                // Sobrescribir método heredado
                int index = it->second;
                std::string new_label = newFunctionLabel();
                vtable.entries[index].function_label = new_label;
                method_id_to_label[vtable.entries[index].method_id] = new_label;
            }
            else
            {
                // Nuevo método
                VTableEntry entry;
                entry.method_name = func->name;
                entry.function_label = newFunctionLabel();
                entry.method_id = newMethodId();

                vtable.entries.push_back(entry);
                vtable.method_to_index[func->name] = vtable.entries.size() - 1;
                method_id_to_label[entry.method_id] = entry.function_label;
            }
        }
    }

    vtables[cls->name] = vtable;
}

void CILGenerator::emitVTableDeclaration(const std::string &class_name, const VTable &vtable)
{
    data_section << "vtable_" << class_name << " = [";
    for (size_t i = 0; i < vtable.entries.size(); ++i)
    {
        if (i > 0)
            data_section << ", ";
        data_section << vtable.entries[i].function_label;
    }
    data_section << "];\n";
}

void CILGenerator::emitMethodDispatchTable()
{
    data_section << "\n// Method dispatch table\n";
    for (const auto &pair : method_id_to_label)
    {
        data_section << "method_" << pair.first << " = " << pair.second << ";\n";
    }
}