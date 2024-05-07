objects := zad1
SRCS := $(wildcard ./src/*.c)
BINS := $(patsubst ./src/%.c, ./build/%, $(SRCS))
CXX := gcc
CFLAGS := -Wall -Wextra -O -lm

.PHONY: all
all: $(objects)

.PHONY: zad1
zad1: clean  zad1printer zad1client

.PHONY: zad1printer
zad1printer: ./build/zad1printer.o
	$(CXX) ./build/zad1printer.o -o ./build/zad1printer 


.PHONY: zad1client
zad1client:  ./build/zad1client.o
	$(CXX) ./build/zad1client.o -o ./build/zad1client 


./build/zad1printer.o:
	 $(CXX)  -c ./src/zad1printer.c -o $@ ${CFLAGS} -lm -lpthread -lrt

	 
./build/zad1client.o:
	 $(CXX)  -c ./src/zad1client.c -o $@ ${CFLAGS} -lm -lpthread -lrt



.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*
	rm -rvf out*.txt pomiar_zad_1.txt