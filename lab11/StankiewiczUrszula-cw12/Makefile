objects := zad1
SRCS := $(wildcard ./src/*.c)
BINS := $(patsubst ./src/%.c, ./build/%, $(SRCS))
CXX := gcc
CFLAGS := -Wall -Wextra -O -lm

.PHONY: all
all: $(objects)

.PHONY: zad1
zad1: clean zad1client zad1server

.PHONY: zad1server
zad1server:  clean ./build/zad1server.o
	$(CXX) ./build/zad1server.o -o ./build/zad1server -lpthread

.PHONY: zad1client
zad1client:  ./build/zad1client.o
	$(CXX) ./build/zad1client.o -o ./build/zad1client -lpthread


./build/zad1server.o:
	 $(CXX)  -c ./src/zad1server.c -o $@ ${CFLAGS} -lm
	 
./build/zad1client.o:
	 $(CXX)  -c ./src/zad1client.c -o $@ ${CFLAGS} -lm



.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*
	rm -rvf out*.txt pomiar_zad_1.txt