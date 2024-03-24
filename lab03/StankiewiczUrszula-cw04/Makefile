objects := zad1 zad2
SRCS := $(wildcard ./src/*.c)
BINS := $(patsubst ./src/%.c, ./build/%, $(SRCS))
CXX := gcc
CFLAGS := -Wall -Wextra -O

.PHONY: all
all: clean $(objects)


.PHONY: zad1
zad1: ./build/zad1.o
	$(CXX) ./build/zad1.o -o ./build/zad1

.PHONY: zad2
zad2: ./build/zad2.o
	$(CXX) ./build/zad2.o -o ./build/zad2


./build/zad1.o:
	$(CXX) -c ./src/zad1.c -o $@ ${CFLAGS}


./build/zad2.o:
	$(CXX) -c ./src/zad2.c -o $@ ${CFLAGS}

.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*
	rm -rvf out*.txt pomiar_zad_1.txt