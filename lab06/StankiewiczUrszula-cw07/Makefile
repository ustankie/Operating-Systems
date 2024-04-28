objects := zad1 zad2sender zad2_counter
SRCS := $(wildcard ./src/*.c)
BINS := $(patsubst ./src/%.c, ./build/%, $(SRCS))
CXX := gcc
CFLAGS := -Wall -Wextra -O -lm

.PHONY: all
all: clean $(objects)

.PHONY: zad1
zad1: clean zad1client zad1server

.PHONY: zad1server
zad1server:  ./build/zad1server.o
	$(CXX) ./build/zad1server.o -o ./build/zad1server

.PHONY: zad1client
zad1client:  ./build/zad1client.o
	$(CXX) ./build/zad1client.o -o ./build/zad1client

.PHONY: zad2
zad2: clean zad2sender zad2_counter

.PHONY: zad2sender
zad2sender: ./build/zad2sender.o
	$(CXX) ./build/zad2sender.o -o ./build/zad2sender -lm

.PHONY: zad2_counter
zad2_counter: ./build/zad2_counter.o
	$(CXX) ./build/zad2_counter.o -o ./build/zad2_counter -lm


./build/zad1server.o:
	 $(CXX)  -c ./src/zad1server.c -o $@ ${CFLAGS} -lm
	 
./build/zad1client.o:
	 $(CXX)  -c ./src/zad1client.c -o $@ ${CFLAGS} -lm


./build/zad2sender.o:
	$(CXX) -c ./src/zad2sender.c -o $@ ${CFLAGS}

./build/zad2_counter.o:
	$(CXX) -c ./src/zad2_counter.c -o $@ ${CFLAGS}

.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*
	rm -rvf out*.txt pomiar_zad_1.txt