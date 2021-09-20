CC = g++
RC = windres.exe
CFLAGS = -Wall -Wextra -std=c++17

FILES = $(shell ls src/*.cpp)
RESOURCES = $(shell ls src/*.rc)

OBJ = $(FILES:.cpp=.o)
C_RESOURCES = $(RESOURCES:.rc=.res)

LIB = -lmingw32 -lSDL2main -lSDL2 -lz -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread
INCLUDE = -I./include
BINARY_NAME = imger.exe

all: $(OBJ) $(C_RESOURCES)
	$(CC) $(CFLAGS) $(OBJ) $(C_RESOURCES) $(LIB) -o $(BINARY_NAME)
	
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

%.res: %.rc
	$(RC) -O coff -i $< -o $@

clean:
	rm $(OBJ) -f
	rm $(C_RESOURCES) -f
	rm $(BINARY_NAME) -f