objects := zad1a zad1b zad2
SRCS := $(wildcard ./src/*.c)
BINS := $(patsubst ./src/%.c, ./build/%, $(SRCS))
CXX := gcc
CFLAGS := -Wall -Wextra -O

.PHONY: all
all: clean $(objects)

.PHONY: test1a
test1a: ./build/zad1a
	./build/zad1a in.txt out.txt
	./build/zad1a out.txt out2.txt

.PHONY: test1b
test1b: ./build/zad1b
	./build/zad1b in.txt out.txt
	./build/zad1b out.txt out2.txt

.PHONY: zad1a
zad1a: ./build/zad1a.o
	$(CXX) ./build/zad1a.o -o ./build/zad1a

.PHONY: zad1b
zad1b: ./build/zad1b.o
	$(CXX) ./build/zad1b.o -o ./build/zad1b

.PHONY: zad2
zad2: ./build/zad2.o
	$(CXX) ./build/zad2.o -o ./build/zad2


./build/zad1a.o:
	$(CXX) -c ./src/zad1a.c -o $@ ${CFLAGS}

./build/zad1b.o:
	$(CXX) -c ./src/zad1b.c -o $@ ${CFLAGS}

./build/zad2.o:
	$(CXX) -c ./src/zad2.c -o $@ ${CFLAGS}

.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*
	rm -rvf out*.txt pomiar_zad_1.txt