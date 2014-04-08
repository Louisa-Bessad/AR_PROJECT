CC = mpicc 
INC = include

all: bin/main

bin/main: obj/main.o obj/init.o 
	${CC} -o $@ $^

obj/init.o: src/init.c $(INC)/init.h
	${CC} -c $< -o $@

obj/main.o: src/main.c $(INC)/main.h
	${CC} -c $< -o $@


clean: 
	rm bin/* obj/*


