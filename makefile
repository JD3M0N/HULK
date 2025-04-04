ifeq ($(OS),Windows_NT)
	# En Windows: solo crear el directorio si no existe.
	MKDIR = if not exist build mkdir build
	CHECK_EXISTS = if not exist build\script.hulk (echo "El archivo build\script.hulk no existe" & exit /b 1)
else
	# En Linux/macOS: -p crea el directorio si no existe y no genera error.
	MKDIR = mkdir -p build
	CHECK_EXISTS = test -f build/script.hulk || (echo "El archivo build/script.hulk no existe" && exit 1)
endif

.PHONY: build run

build:
	$(MKDIR)
	$(CHECK_EXISTS)
	@echo "Construcción completada."

run: build
	@echo "Ejecutar: (acción pendiente)"
