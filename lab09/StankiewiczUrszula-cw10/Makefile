objects := zad1
SRCS := $(wildcard ./src/*.c)
BINS := $(patsubst ./src/%.c, ./build/%, $(SRCS))
CXX := gcc
CFLAGS := -Wall -Wextra -O -lm

.PHONY: all
all: clean $(objects)


.PHONY: zad1
zad1: ./build/zad1.o
	$(CXX) ./build/zad1.o -o ./build/zad1 



./build/zad1.o:
	 $(CXX)  -c ./src/zad1.c -o $@ ${CFLAGS} -lm -lpthread -lrt

	 

.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*
	rm -rvf out*.txt pomiar_zad_1.txt