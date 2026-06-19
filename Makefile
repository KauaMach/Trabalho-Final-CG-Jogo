CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I./src/bibliotecas -I./src

# Detecção de plataforma: Windows / macOS / Linux
ifeq ($(OS),Windows_NT)
    LIBS = -lmingw32 -lfreeglut -lopengl32 -lglu32 -lSDL2main -lSDL2 -lSDL2_mixer -mconsole
    TARGET = imunidade_jogo.exe
    CLEAN_COMMAND = @if exist src\\main.o del /Q src\\main.o && if exist src\\classes\\*.o del /Q src\\classes\\*.o && if exist $(TARGET) del /Q $(TARGET)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        # macOS — Frameworks do sistema + SDL2 via Homebrew
        LIBS = -framework OpenGL -framework GLUT -framework Cocoa -lSDL2 -lSDL2_mixer
        TARGET = imunidade_jogo
    else
        # Linux — Pacotes do sistema
        LIBS = -lglut -lGL -lGLU -lSDL2 -lSDL2_mixer
        TARGET = imunidade_jogo
    endif
    CLEAN_COMMAND = rm -f src/main.o src/classes/*.o $(TARGET)
endif

# Lista de ficheiros-fonte
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
	$(CLEAN_COMMAND)
	@echo Limpeza concluida!

.PHONY: all clean