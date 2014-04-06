CC = mpicc 
CFLAGS = -03
INC = include

all: bin/main

bin/main: obj/init.o obj/main.o
	${CC} ${CFLAGS} -o $@ $^

obj/main.o: src/main.c $(INC)/main.h
	${CC} -c $^

obj/init.o: src/init.c $(INC)/init.h
	${CC} -c $^ 

clean: 
	rm bin/* obj/*


