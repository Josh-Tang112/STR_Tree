CC = g++ 
LDLIBS = -lm
CFLAGS = -Itree -ggdb3

all: debug

debug: debug.cpp
	$(CC) debug.cpp $(LDLIBS) $(CFLAGS) -o debug

clean:
	rm debug
