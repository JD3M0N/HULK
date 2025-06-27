# Sistema de Manejo de Errores para el Compilador HULK

## Descripción General

Este sistema proporciona un manejo robusto y profesional de errores para el compilador HULK, diseñado siguiendo las mejores prácticas de ingeniería de compiladores.

## Características Principales

### 1. Categorización de Errores

El sistema clasifica los errores en los siguientes tipos:

- **LEXICAL_ERROR**: Errores durante el análisis léxico (tokens inválidos, caracteres no reconocidos)
- **SYNTAX_ERROR**: Errores durante el análisis sintáctico (estructura inválida del programa)
- **SEMANTIC_ERROR**: Errores de semántica (uso incorrecto de identificadores, etc.)
- **TYPE_ERROR**: Errores de tipos (incompatibilidad de tipos, inferencia fallida)
- **SCOPE_ERROR**: Errores de ámbito (variables no declaradas, identificadores fuera de scope)
- **CODEGEN_ERROR**: Errores durante la generación de código
- **RUNTIME_ERROR**: Errores durante la ejecución
- **INTERNAL_ERROR**: Errores internos del compilador

### 2. Niveles de Severidad

- **WARNING**: Advertencias que no impiden la compilación
- **ERROR**: Errores que impiden la compilación exitosa
- **FATAL**: Errores críticos que requieren abortar inmediatamente

### 3. Información de Ubicación

Cada error incluye:
- Nombre del archivo fuente
- Número de línea
- Número de columna (cuando esté disponible)
- Contexto del código donde ocurrió el error

## Arquitectura del Sistema

### Clase ErrorHandler

Implementa el patrón Singleton para centralizar el manejo de errores:

```cpp
ErrorHandler& errorHandler = ErrorHandler::getInstance();
```

### Clase ErrorMessage

Encapsula toda la información de un error:
- Tipo de error
- Severidad
- Mensaje descriptivo
- Ubicación en el código fuente
- Contexto del código

### Clase CompilationException

Excepción personalizada para errores que requieren interrumpir el flujo normal:

```cpp
throw CompilationException(ErrorType::SYNTAX_ERROR, "mensaje", location);
```

## Uso del Sistema

### Reportar Errores

```cpp
// Usando el ErrorHandler directamente
ErrorHandler::getInstance().reportError(ErrorType::TYPE_ERROR, 
                                      "Tipo incompatible", 
                                      SourceLocation("file.hulk", 10, 5),
                                      "x = y + z");

// Usando macros de conveniencia
TYPE_ERROR("Tipo incompatible", SourceLocation("file.hulk", 10, 5), "x = y + z");

// Con contexto automático
REPORT_ERROR_WITH_CONTEXT(ErrorType::TYPE_ERROR, "mensaje", location);
```

### Verificar Estado de Errores

```cpp
if (errorHandler.hasErrors()) {
    // Hay errores
}

if (errorHandler.shouldAbortCompilation()) {
    // Se debe abortar la compilación
}
```

### Generar Reportes

```cpp
// Imprimir errores en stderr
errorHandler.printErrors();

// Imprimir resumen
errorHandler.printSummary();

// Generar reporte completo
std::string report = errorHandler.generateReport();

// Generar reporte HTML
std::string htmlReport = ErrorUtils::generateHTMLReport(errorHandler);
```

## Integración en el Pipeline de Compilación

El sistema está integrado en cada fase del compilador:

1. **Análisis Léxico**: Detecta tokens inválidos y caracteres no reconocidos
2. **Análisis Sintáctico**: Detecta errores de estructura del programa
3. **Resolución de Nombres**: Detecta identificadores no declarados
4. **Inferencia de Tipos**: Detecta incompatibilidades de tipos
5. **Generación de Código**: Detecta errores durante la generación de CIL/MIPS

## Configuración

### Límite de Errores

```cpp
errorHandler.setMaxErrors(20); // Máximo 20 errores antes de abortar
```

### Limpieza del Estado

```cpp
errorHandler.clear(); // Limpiar todos los errores registrados
```

## Utilidades Adicionales

### ErrorUtils

Proporciona funciones auxiliares:

- `getLineContext()`: Obtiene el contexto de una línea específica
- `highlightPosition()`: Resalta una posición en el código
- `validateFile()`: Valida que un archivo existe y es legible
- `generateHTMLReport()`: Genera reportes en formato HTML

## Ejemplo de Salida

```
=== Iniciando compilación de test.hulk ===
=== Fase de Análisis Sintáctico ===
[ERROR] Syntax Error at test.hulk:15: Token inesperado '{'
    Context: function foo() {

=== COMPILATION SUMMARY ===
Errors: 1
Compilation FAILED due to errors.
```

## Beneficios del Sistema

1. **Diagnóstico Preciso**: Ubicación exacta de errores con contexto
2. **Categorización Clara**: Errores organizados por tipo y fase
3. **Recuperación de Errores**: El compilador continúa después de errores no fatales
4. **Reportes Detallados**: Información completa para debugging
5. **Integración Transparente**: Fácil de usar en todo el compilador
6. **Extensibilidad**: Fácil agregar nuevos tipos de errores
7. **Configurabilidad**: Límites y comportamiento personalizables

## Códigos de Salida

- `0`: Compilación exitosa
- `1`: Error de argumentos o archivo no encontrado
- `2`: Error en análisis sintáctico
- `3`: Error en resolución de nombres
- `4`: Error en inferencia de tipos
- `5`: Error en generación de código CIL
- `6`: Error en generación de código MIPS
- `7`: Excepción no controlada durante compilación
- `8`: Error interno inesperado

Este sistema proporciona una base sólida para el desarrollo y mantenimiento del compilador HULK, facilitando la detección, diagnóstico y corrección de errores en el código fuente.
