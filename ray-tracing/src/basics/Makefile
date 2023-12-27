CC=g++
CPP_LIBS=-lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl
INCLUDE=../../include
OUTPUT=basics
SOURCE=basics.cpp gl.c shader.cpp stb_image.cpp
all:
	$(CC) $(CPP_LIBS) -I $(INCLUDE) -o $(OUTPUT) $(SOURCE)

cc:
	bear -- $(CC) $(CPP_LIBS) -I $(INCLUDE) -o $(OUTPUT) $(SOURCE)
	mv compile_commands.json ..
clean:
	rm basics
