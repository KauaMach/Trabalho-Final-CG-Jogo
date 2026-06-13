CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O0 -I./src/bibliotecas -I./src
LIBS = -lmingw32 -lfreeglut -lopengl32 -lglu32 -lSDL2main -lSDL2 -lSDL2_mixer -mconsole
TARGET = imunidade_jogo.exe

# Lista de arquivos fontes (.cpp)
SRC = src/main.cpp \
      src/classes/renderer.cpp \
      src/classes/renderer_menu.cpp \
      src/classes/renderer_player.cpp \
      src/classes/renderer_virus1.cpp \
      src/classes/renderer_virus2.cpp \
      src/classes/renderer_virus3.cpp \
      src/classes/collision.cpp \
      src/classes/ai_boids.cpp \
      src/classes/ai_fsm.cpp \
      src/classes/particles.cpp \
      src/classes/audio.cpp

# Mapeia automaticamente a lista de fontes para arquivos de objeto (.o)
# Mantendo os objetos organizados na mesma estrutura de pastas
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

# Regra de Linkagem final: junta todos os arquivos .o gerados no executável
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LIBS)
	@echo Executavel gerado com sucesso!

# Regra de Compilação genérica: compila cada .cpp em seu respectivo .o separadamente
%.o: %.cpp
	@echo Compilando $<...
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo Limpando arquivos temporarios e executavel...
	@if exist src\main.o del /Q src\main.o
	@if exist src\classes\*.o del /Q src\classes\*.o
	@if exist $(TARGET) del /Q $(TARGET)
	@echo Limpeza concluida!

.PHONY: all clean