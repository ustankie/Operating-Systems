objects := staticFile sharedFile dynamicFile
SRCS := $(wildcard ./src/*.c)
BINS := $(patsubst ./src/%.c, ./build/%, $(SRCS))
CXX := gcc
CFLAGS := -Wall -Wextra 

.PHONY: all
all:  clean $(objects)

staticFile: static ./src/client.c
	$(CXX) -o ./build/client_static ./src/client.c -L. ./build/libcollatz.a

sharedFile: shared ./src/client.c
	$(CXX) ./src/client.c -lcollatz  -L ./build -Wl,-rpath=./build/ -o ./build/client_shared

dynamicFile: dynamic ./src/client.c
	$(CXX) ./src/client.c -L ./build -Wl,-rpath=./build -o ./build/client_dynamic -ldl -DDYNAMIC



.PHONY: static
static: ./build/collatz_static.o ./include/collatz.c 
		ar rcs ./build/libcollatz.a ./build/collatz_static.o 
		

.PHONY: shared
shared: ./build/collatz_shared.o ./include/collatz.c 
		$(CXX)  -shared -fPIC -o ./build/libcollatz.so  ./build/collatz_shared.o
		

.PHONY: dynamic
dynamic: ./build/collatz_dynamic.o ./include/collatz.c 
		$(CXX)  -shared -fPIC -o ./build/libcollatz.so  ./build/collatz_dynamic.o 

./build/collatz_shared.o: ./include/collatz.c 
		$(CXX) -fPIC -c ./include/collatz.c -o ./build/collatz_shared.o		


./build/collatz_static.o: ./include/collatz.c
		$(CXX) -c ./include/collatz.c -o ./build/collatz_static.o ${CFLAGS}

./build/collatz_dynamic.o: ./include/collatz.c 
	$(CXX) -fPIC -c ./include/collatz.c -o ./build/collatz_dynamic.o	

.PHONY: clean
clean:
	@echo "Cleaning ..."
	rm -rvf ./build/*