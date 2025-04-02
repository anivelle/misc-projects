CFLAGS=-ggdb $(shell pkg-config --cflags csfml-all)
LDFLAGS=-lm $(shell pkg-config --libs csfml-all)
all:
	gcc $(LDFLAGS) $(CFLAGS) -o clay-testing src/main.c
