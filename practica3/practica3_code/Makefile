CC = mpicc
CFLAGS = -Wall -Wextra -g

# Opciones de enlazado
LDFLAGS = -lSDL2

# Nombre del ejecutable
EXECUTABLE = lifeGame

# Archivos .c que hay en el directorio
SOURCES = $(wildcard *.c)

# Regla para enlazar los archivos objeto y generar el ejecutable
all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Regla para limpiar los archivos objeto y el ejecutable
clean:
	rm -f $(EXECUTABLE)
