CC = mpicc 
INC = include

all: bin/main

bin/main: obj/main.o obj/init.o obj/tools.o
	${CC} -o $@ $^

obj/init.o: src/init.c $(INC)/init.h $(INC)/struct.h $(INC)/define.h
	${CC} -c $< -o $@

obj/main.o: src/main.c $(INC)/main.h $(INC)/struct.h $(INC)/define.h $(INC)/tools.h src/tools.c $(INC)/init.h src/init.c
	${CC} -c $< -o $@

obj/tools.o: src/tools.c $(INC)/tools.h $(INC)/struct.h $(INC)/define.h $(INC)/main.h src/main.c
	${CC} -c $< -o $@

clean: 
	rm -f bin/* obj/*


