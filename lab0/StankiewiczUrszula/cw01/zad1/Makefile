objects := ./build/countdown
SRCS := $(wildcard ./src/*.c)
BINS := $(patsubst ./src/%.c, ./build/%, $(SRCS))
CXX := gcc
CFLAGS := -g -Wall -Wextra 

.PHONY: all
all: $(objects)

./build/countdown: ./build/countdown.o
	${CXX} ./build/countdown.o -o $@ ${CFLAGS}

./build/countdown.o: ./src/countdown.c
	${CXX} -c ./src/countdown.c -o $@ ${CFLAGS}

.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*.o ${BINS}