#include "include/init.h"


int init_coordinateur(int my_rank){
	int msg;

	int rand_values[N*10];
	MPI_Send(&msg,1, MPI_INT,(myrank+1),tag, MPI_COMM_WORLD);

	return 0;	
}


int init_node(int my_rank){

	coordonnee *id_coord;
	id_coord = (coordonnee *)malloc(sizeof(coordonnee));
	srand(getpid());
	init_struct(rand()%1000, rand()%1000, id_coord);

	return 0;
}

coordonnee init_struct(int x, int y){

	coordonnee res;
	res.x = x;
	res.y = y;

	return res;

}