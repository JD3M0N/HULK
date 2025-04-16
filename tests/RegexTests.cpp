// tests/RegexTests.cpp
#include <iostream>
#include <string>
#include <vector>
#include "../src/regex/regex.h" // Asegúrate de que la ruta sea la correcta

struct TestCase
{
    std::string pattern;
    std::string text;
    bool expected;
};

int main()
{
    std::cout << "Pruebas del motor de expresiones regulares:" << std::endl;

    // Define varios casos de prueba
    std::vector<TestCase> tests = {
        {"a*b", "b", true},
        {"a*b", "ab", true},
        {"a*b", "aaab", true},
        {"a*b", "a", false},
        {"a*b", "ba", false},
        {"a*b", "aaaba", false},
        {".*", "hola", true},
        {"a*.", "aaaX", true},
        {"a*.", "X", true},
        {"a*.", "aa", false},
        {"a*ba", "aaaaba", true},
        {"ab*c", "ac", true},
        {"ab*c", "abc", true},
        {"ab*c", "abbc", true},
        {"ab*c", "abdc", false},
        {"a*b*c*", "", true} // La cadena vacía debe coincidir, ya que cada asterisco permite 0 repeticiones
    };

    int passed = 0;
    for (size_t i = 0; i < tests.size(); i++)
    {
        Regex regex(tests[i].pattern);
        bool result = regex.matches(tests[i].text);
        std::cout << "Patrón: \"" << tests[i].pattern
                  << "\"  Texto: \"" << tests[i].text
                  << "\"  Esperado: " << (tests[i].expected ? "true" : "false")
                  << "  Obtenido: " << (result ? "true" : "false") << std::endl;
        if (result == tests[i].expected)
        {
            passed++;
        }
    }
    std::cout << "Pruebas superadas: " << passed << " de " << tests.size() << std::endl;

    return 0;
}
