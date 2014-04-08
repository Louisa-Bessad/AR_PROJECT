CC = mpicc 
INC = include

all: bin/main

bin/main: obj/main.o obj/init.o obj/tools.o
	${CC} -o $@ $^

obj/init.o: src/init.c $(INC)/init.h
	${CC} -c $< -o $@

obj/main.o: src/main.c $(INC)/main.h
	${CC} -c $< -o $@

obj/tools.o: src/tools.c $(INC)/tools.h
	${CC} -c $< -o $@

clean: 
	rm bin/* obj/*


