CC=g++
CPP_LIBS=-lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl
INCLUDE=../include
all:
	$(CC) $(CPP_LIBS) -I $(INCLUDE) -o ray-tracing ray-tracing.cpp gl.c

cc:
	bear -- $(CC) $(CPP_LIBS) -I $(INCLUDE) -o ray-tracing ray-tracing.cpp gl.c
clean:
	rm ray-tracing
