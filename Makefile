CC = g++ 
LDLIBS = -lsfml-graphics -lsfml-window -lsfml-system -lm
CFLAGS = -Iphysics -Ihelper -ggdb3

all: debug

debug: debug.cpp
	$(CC) debug.cpp $(LDLIBS) $(CFLAGS) -o debug

clean:
	rm debug
