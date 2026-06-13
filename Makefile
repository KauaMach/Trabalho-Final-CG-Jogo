CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src/bibliotecas -I./src
LIBS = -lmingw32 -lfreeglut -lopengl32 -lglu32 -lSDL2main -lSDL2 -lSDL2_mixer -mconsole
TARGET = imunidade_jogo.exe

SRC = src/main.cpp \
      src/classes/renderer.cpp \
      src/classes/collision.cpp \
      src/classes/ai_boids.cpp \
      src/classes/ai_fsm.cpp \
      src/classes/particles.cpp \
      src/classes/audio.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	@if exist $(TARGET) del /Q $(TARGET)
	@echo Limpeza concluida!

.PHONY: all clean