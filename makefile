GXX=clang++
LIBDIR=
LIB= -lglfw -lm -lGL -lGLEW -lassimp -lstdc++fs 
INC= 
CFLAGS= -Wall -std=c++17
SRC=src

OBJECTS = main.o cloth.o point.o mesh.o stb_image.o

all: rm_noin  main
main: main.o $(OBJECTS)
	$(GXX) $(LIBDIR) -o noin $(OBJECTS) $(LIB)

stb_image.o: $(SRC)/stb_image.cc
	$(GXX) $(CFLAGS) $(INC) -c $(SRC)/stb_image.cc -o stb_image.o
cloth.o: $(SRC)/cloth.cc
	$(GXX) $(CFLAGS) $(INC) -c $(SRC)/cloth.cc -o cloth.o
point.o: $(SRC)/point.cc
	$(GXX) $(CFLAGS) $(INC) -c $(SRC)/point.cc -o point.o
mesh.o: $(SRC)/mesh.cc
	$(GXX) $(CFLAGS) $(INC) -c $(SRC)/mesh.cc -o mesh.o
main.o: $(SRC)/main.cc
	$(GXX) $(CFLAGS) $(INC) -c $(SRC)/main.cc -o main.o


rm_noin:
	rm -f noin

clean:
	rm -f *.o 
	rm -f noin
