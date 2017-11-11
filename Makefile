# Project: MyFarm

CPP  = g++
CC   = gcc
BIN  = myFarm

OBJ  = main.o tractor.o mesh.o drawScene.o
LINKOBJ  = main.o tractor.o mesh.o drawScene.o
LIBS = -L/usr/X11R6/lib -framework OpenGL -framework GLUT  -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lm
INCS = -I. -I/usr/X11R6/include -D_THREAD_SAFE  -I/usr/local/include/SDL2 -I/usr/local/Cellar -I/usr/local/Cellar/glew/2.1.0/include
CXXINCS=#
CXXFLAGS = $(CFLAGS)
CFLAGS = $(INCS)
RM = rm -f

all: $(BIN)


clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

drawScene.o: drawScene.cpp
	$(CPP) -c drawScene.cpp -o drawScene.o $(CXXFLAGS)

tractor.o: tractor.cpp
	$(CPP) -c tractor.cpp -o tractor.o $(CXXFLAGS)

mesh.o: mesh.cpp
	$(CPP) -c mesh.cpp -o mesh.o $(CXXFLAGS)
