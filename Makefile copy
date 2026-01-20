# DoomLike Makefile (Adaptado para Windows/MinGW)

CXX      := g++
# Flags comuns. Importante: Windows precisa de -static para embutir algumas libs ou você precisará das DLLs
CXXFLAGS := -g -O0 -Wall -Wextra -Iinclude 

# Bibliotecas no Windows tem nomes diferentes
# Certifique-se de ter instalado o GLEW e FreeGLUT no seu MinGW
LDLIBS   := -lglew32 -lfreeglut -lopengl32 -lglu32

TARGET   := DoomLike.exe
BUILD_DIR := build

SRC_DIR   := src
MAIN      := main.cpp

# O comando 'shell find' não funciona bem no CMD do Windows. 
# Vamos listar as pastas explicitamente com wildcard (mais seguro para Windows)
SRCS := $(wildcard src/core/*.cpp) \
        $(wildcard src/graphics/*.cpp) \
        $(wildcard src/input/*.cpp) \
        $(wildcard src/level/*.cpp) \
        $(wildcard src/utils/*.cpp) \
        $(MAIN)

# Substitui .cpp por .o e ajusta o caminho para build/
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run dirs

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): dirs $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDLIBS)

# Regra de compilação
$(BUILD_DIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Cria diretório build e subdiretórios se não existirem (sintaxe Windows)
dirs:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(BUILD_DIR)\src\core" mkdir "$(BUILD_DIR)\src\core"
	@if not exist "$(BUILD_DIR)\src\graphics" mkdir "$(BUILD_DIR)\src\graphics"
	@if not exist "$(BUILD_DIR)\src\input" mkdir "$(BUILD_DIR)\src\input"
	@if not exist "$(BUILD_DIR)\src\level" mkdir "$(BUILD_DIR)\src\level"
	@if not exist "$(BUILD_DIR)\src\utils" mkdir "$(BUILD_DIR)\src\utils"

run: all
	$(BUILD_DIR)\$(TARGET)

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)