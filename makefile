rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SRC = $(call rwildcard, src/, *.c) #$(wildcard src/*.cpp) $(wildcard src/engine/*.cpp)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
CFLAGS = --std=c11 -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-unused-value -Isrc/
NAME = 1c
OUT =

LDFLAGS = -Wl,-allow-multiple-definition -static -static-libgcc -mwindows
LIBS = 
CC = 
ifeq ($(OS),Windows_NT)
	OUT = $(NAME).exe
	CC += clang
	LIBS = -lraylib -lopengl32 -lgdi32
else
	OUT = $(NAME)
	CC += clang
	LIBS = -lraylib -lglfw3 -lGL -lXxf86vm -lXext -lX11 -lXrandr -lXinerama -lXcursor -lm -lpthread -ldl 
endif

all: $(OUT)

$(OUT): $(OBJ)
	@ test -d bin || mkdir bin
	$(CC) -o bin/$(OUT) $(CFLAGS) $(OBJ) $(LIBS) $(LDFLAGS)

obj/%.o: src/%.c
	@ test -d $(@D) || mkdir $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -r obj/*
	rm bin/$(OUT)

run:
	cd bin; ./$(OUT)
