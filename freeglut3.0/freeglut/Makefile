# Compilador e flags
CXX = g++
CXXFLAGS = -I./include -Wall -g
LDFLAGS = -L./lib/x64 -lfreeglut -lopengl32 -lglu32

# Fontes e alvo
SRCS = main.cpp jogo.cpp render.cpp util.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = sokoban_Coleta.exe

all:	$(TARGET)

$(TARGET):	$(SRCS)
	$(CXX)	$(CXXFLAGS) -o	$(TARGET)	$(SRCS)	$(LDFLAGS)

clean:
	del /Q *.o *.exe

.PHONY:	all clean

