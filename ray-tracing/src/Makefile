CC=g++
CPP_LIBS=-lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl
INCLUDE=../include
all:
	$(CC) $(CPP_LIBS) -I $(INCLUDE) -o ray-tracing ray-tracing.cpp gl.c shader.cpp

cc:
	bear -- $(CC) $(CPP_LIBS) -I $(INCLUDE) -o ray-tracing ray-tracing.cpp gl.c shader.cpp
	mv compile_commands.json ..
clean:
	rm ray-tracing
