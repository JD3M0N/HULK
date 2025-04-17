#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// Incluimos directamente la implementación del parser
#include "../src/parser/Parser.cpp"

using namespace std;

// Prueba que la expresión válida se parsea sin lanzar excepciones y retorna un árbol no nulo
void test_valid(const string &expr)
{
    try
    {
        LRParser parser(expr);
        auto tree = parser.parse();
        if (!tree)
        {
            cerr << "[FAIL] parse devolvió null para \"" << expr << "\"" << endl;
            exit(1);
        }
        cout << "[PASS] " << expr << endl;
    }
    catch (const exception &e)
    {
        cerr << "[FAIL] \"" << expr << "\" lanzó excepción: " << e.what() << endl;
        exit(1);
    }
}

// Prueba que la expresión inválida lanza runtime_error
void test_invalid(const string &expr)
{
    try
    {
        LRParser parser(expr);
        parser.parse();
        cerr << "[FAIL] \"" << expr << "\" no lanzó excepción." << endl;
        exit(1);
    }
    catch (const runtime_error &)
    {
        cout << "[PASS] expresión inválida \"" << expr << "\" lanzó excepción como se esperaba." << endl;
    }
    catch (const exception &e)
    {
        cerr << "[FAIL] expresión inválida \"" << expr << "\" lanzó excepción incorrecta: " << e.what() << endl;
        exit(1);
    }
}

int main()
{
    // Casos válidos
    test_valid("1");
    test_valid("1+2");
    test_valid("(3*4)-5");
    test_valid("-6");
    test_valid("7*(8+9)/2");

    // Casos inválidos
    test_invalid("");
    test_invalid("1+");
    test_invalid("()");

    cout << "Todos los tests pasaron correctamente." << endl;
    return 0;
}
