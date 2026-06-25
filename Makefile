CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -Isrc

# Configurações de Linker (Multiplataforma)
LIBS_LINUX = -lGL -lGLU -lglut -lGLEW -lSDL2 -lSDL2_mixer
LIBS_MAC = -framework OpenGL -framework GLUT -lGLEW -lSDL2 -lSDL2_mixer
LIBS_WIN = -lopengl32 -lglu32 -lfreeglut -lglew32 -lSDL2 -lSDL2_mixer

SRC_FILES = $(wildcard src/*.cpp) $(wildcard src/core/*.cpp) $(wildcard src/scenes/*.cpp) $(wildcard src/entities/*.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)
TARGET = imunidade

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	LIBS = $(LIBS_LINUX)
endif
ifeq ($(UNAME_S),Darwin)
	LIBS = $(LIBS_MAC)
endif

# Regra padrao
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LIBS)

# Compilacao Cruzada para Windows (usando MinGW no Linux/Mac)
win: CXX = x86_64-w64-mingw32-g++
win: LIBS = $(LIBS_WIN)
win: TARGET = imunidade.exe
win: clean $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) imunidade imunidade.exe
