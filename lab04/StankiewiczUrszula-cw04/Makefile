objects := zad1 zad2sender zad2catcher
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
zad2: zad2sender zad2catcher

.PHONY: zad2sender
zad2sender: ./build/zad2sender.o
	$(CXX) ./build/zad2sender.o -o ./build/zad2sender

.PHONY: zad2catcher
zad2catcher: ./build/zad2catcher.o
	$(CXX) ./build/zad2catcher.o -o ./build/zad2catcher


./build/zad1.o:
	$(CXX) -c ./src/zad1.c -o $@ ${CFLAGS}


./build/zad2sender.o:
	$(CXX) -c ./src/zad2sender.c -o $@ ${CFLAGS}

./build/zad2catcher.o:
	$(CXX) -c ./src/zad2catcher.c -o $@ ${CFLAGS}

.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*
	rm -rvf out*.txt pomiar_zad_1.txt