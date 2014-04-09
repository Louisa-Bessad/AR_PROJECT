#include "../include/init.h"


int init_coordinateur(int my_rank){
  int msg[2];
  int rand_values[N*10];
  MPI_Status status;

  MPI_Send(msg, 2, MPI_INT, 1, __TAG_BEGIN_INSERT, MPI_COMM_WORLD);
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

coordonnee init_struct(int x, int y){
  coordonnee res;
	res.x = x;
	res.y = y;
  return res;
}

borne init_borne( int inf_x, int sup_x, int inf_y, int sup_y ){
  borne res;
  res.bix = inf_x;
  res.bsx = sup_x;
  res.biy = inf_y;
  res.bsy = sup_y;
  return res;
}

nei_list* init_list (int rank, int coord_x, int coord_y, int bix, int bsx, int biy, int bsy){
  nei_list* racine = (nei_list*)malloc(sizeof(nei_list));
  printf("\t\t racine -> rank : %d coord_x %d coord_y %d bix %d bsx %d biy %d bsy %d \n", rank, coord_x,  coord_y,  bix,  bsx,  biy,  bsy );
  if(racine != NULL){
    racine->rank = rank;
    racine->point = init_struct(coord_x, coord_y);
    racine->inter = init_borne(bix, bsx, biy, bsy);
    racine->next = racine;
    racine->prev = racine;

    return racine;
  }
  return NULL;
}
