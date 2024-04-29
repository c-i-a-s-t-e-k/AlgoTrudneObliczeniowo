# Nazwa pliku wykonywalnego, który zostanie stworzony
TARGET = program

# Ścieżka do kompilatora
CXX = g++

# Opcje kompilatora
CXXFLAGS = -std=c++11 -Wall -Wextra

# Plik źródłowy
SOURCE = main.cpp

# Plik obiektowy, który zostanie utworzony podczas kompilacji
OBJECT = $(SOURCE:.cpp=.o)

# Reguła domyślna
all: $(TARGET)

# Reguła linkowania
$(TARGET): $(OBJECT)
	$(CXX) $(CXXFLAGS) $(OBJECT) -o $(TARGET)

# Reguła kompilacji
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Reguła czyszczenia
clean:
	rm -f $(TARGET) $(OBJECT)

# Reguła 'phony' dla uniknięcia konfliktów z plikami o nazwach takich jak reguły
.PHONY: all clean