.PHONY: clean

CC = g++
CFLAGS = -g -Wall -I./include -lncurses

default: hw2

hw2: src/main.cpp build/agent.o build/game.o
	$(CC) $(CFLAGS) -g -o $@ $^

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) -g -c -o $@ $<


clean:
	rm -rf build
	rm -rf hw2
	mkdir build
