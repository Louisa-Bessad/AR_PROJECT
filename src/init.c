#include "../include/init.h"


int init_coordinateur(int my_rank){
  int msg[2];
  int rand_values[N*10];
  MPI_Status status;

  MPI_Send(msg, 2, MPI_INT, 1, __TAG_BEGIN_INSERT, MPI_COMM_WORLD);

  /*MPI_Recv(&msg, 2, MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &status);*/

  printf("%d : sortie de coordinateur\n", my_rank);

  bootstrap.x = msg[0];
  bootstrap.y = msg[1];


  return 0;	
}


int init_node(int my_rank){

  srand(getpid());
  id_coord.x = rand()%1000;
  id_coord.y = rand()%1000;
  if(my_rank==1){
    inter.bix = 0;
    inter.biy= 0;
    inter.bsx = 1000;
    inter.bsy = 1000;
  }
  printf("%d : sortie de init_node, coordx = %d, coordy = %d\n", my_rank, id_coord.x, id_coord.y);
  return 0;
}

void init_struct(int x, int y, coordonnee *res){

	res->x = x;
	res->y = y;

}
