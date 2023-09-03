#Notes just in case
#x86_64-w64-mingw32-gcc main.cpp -I/usr/x86_64-w64-mingw32/include/SDL2 -L/usr/x86_64-w64-mingw64/lib -lmingw32 -lSDL2main -lSDL2 -o ./t.exe
#x86_64-w64-mingw32-gcc -o main64.exe main.c
#g++ ./main_test.cpp -lSDL -o bin

CFLAGS    = -O2 -Wall
BINARY    = reading
CROSSCOMPILE = no
EXTLIBS =

### call with make all CROSSCOMPILE=yes to cross compile
### otherwise, assumes a *nix enviornment (tested on OSX and Arch Linux) 
ifeq ($(CROSSCOMPILE), yes)
	CC = x86_64-w64-mingw32-gcc
	X = .exe
    EXTLIBS += -lmingw32
else
	CC = g++
	X  =  
endif

EXTLIBS += -lSDL2main -lSDL2 -lSDL2_ttf

all: $(BINARY)

$(BINARY): main.o
	$(CC) $? $(EXTLIBS) -o $(@)$(X)

main.o: main.cpp
	$(CC) -c $(CFLAGS) $?

clean: rm -f $(BINARY) main.o

.PHONY: all clean
