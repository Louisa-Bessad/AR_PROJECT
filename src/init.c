#include "../include/init.h"


int init_coordinateur(int my_rank){
  int msg[2];

  int rand_values[N*10];
  MPI_Send(&msg, 2, MPI_INT,(my_rank+1),tag, MPI_COMM_WORLD);
  MPI_Recv(&msg, 2, MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD);

  bootstrap.x = msg[0];
  bootstrap.y = msg[1];


  return 0;	
}


int init_node(int my_rank){

  srand(getpid());
  id_coord.x = rand()%1000;
  id_coord.y = rand()%1000;
  if(my_rank==1){
    inter.borne_inf_x = 0;
    inter.borne_inf_y = 0;
    inter.borne_sup_x = 1000;
    inter.borne_sup_y = 1000;
  }

  return 0;
}

void init_struct(int x, int y, coordonnee *res){

	res->x = x;
	res->y = y;

}
