CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O0 -I./src/bibliotecas -I./src
LIBS = -lmingw32 -lfreeglut -lopengl32 -lglu32 -lSDL2main -lSDL2 -lSDL2_mixer -mconsole
TARGET = imunidade_jogo.exe

# LISTA ATUALIZADA COM OS DOIS FICHEIROS SEPARADOS
SRC = src/main.cpp \
      src/classes/renderer.cpp \
      src/classes/renderer_menu.cpp \
      src/classes/renderer_player.cpp \
      src/classes/renderer_virus1.cpp \
      src/classes/renderer_virus2.cpp \
      src/classes/renderer_virus3.cpp \
      src/classes/renderer_virus4.cpp \
      src/classes/collision.cpp \
      src/classes/ai_boids.cpp \
      src/classes/ai_fsm.cpp \
      src/classes/particles.cpp \
      src/classes/audio.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LIBS)
	@echo Executavel gerado com sucesso!

%.o: %.cpp
	@echo Compilando $<...
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo Limpando arquivos temporarios...
	@if exist src\main.o del /Q src\main.o
	@if exist src\classes\*.o del /Q src\classes\*.o
	@if exist $(TARGET) del /Q $(TARGET)
	@echo Limpeza concluida!

.PHONY: all clean