#include "../include/main.h"
#include "../include/init.h"
#include "../include/tools.h"

void coordinateur(int my_rank){
  int cpt_nodes = 1;
  while(cpt_nodes<N){
    MPI_Recv(&msg, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    switch (status.MPI_TAG){
    case __TAG_NODE_INSERTED:
      cpt_nodes++;
      MPI_Send(&id_coord,2, MPI_INT, cpt_nodes, __TAG_BEGIN_INSERT, MPI_COMM_WORLD);
      break;

    default: 
      break;
    }
  }
}

int ask_insertion(int my_rank){
  int msg, msg_coord[2];
  MPI_Status status;
  int dif_borne_x, dif_borne_y,  moitie_x, moitie_y;
  borne new_borne;

  while(1){
    MPI_Recv(&msg_coord, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 

    switch(status.MPI_TAG){ 
    case __TAG_BEGIN_INSERT:
      MPI_Send(&id_coord,2, MPI_INT, 1, __TAG_INSERT_ME, MPI_COMM_WORLD); 
      MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
      /*if (status.MPI_TAG == __TAG_END_INSTERT)*/
      break;

    case __TAG_INSERT_ME:
      if((inter.bix < msg_coord.x) && (msg_coord.x < inter.bsx)){
	if((inter.biy < msg_coord.y) && (msg_coord.y < inter.bsy)){

	  dif_borne_x = dif(inter.bsx, inter.bix);
	  dif_borne_y = dif(inter.bsy, inter.biy);


	  if(dif_borne_x >= dif_borne_y){
	    /*rectangle horizontal*/
	    moitie_x = dif_borne_x/2;
	    if (id_coord.x > moitie_x){
	      send_response(inter.bix, moitie_x, status.MPI_SOURCE);
	      send_response(inter.biy, inter.bsy, status.MPI_SOURCE);
	      /*le nouveau noeud sera dans [bix ; moitie]*/
	    }
	    else{
	      send_response(moitie_x, inter.bsx, status.MPI_SOURCE);
	      send_response(inter.biy, inter.bsy, status.MPI_SOURCE);
	      /*le nouveau noeud sera dans [moitie ; bsx]*/
	    }
  	  }

	  else{
	    /*rectangle vertical*/
	    moitie_y = dif_borne_y/2;
	    if (id_coord.y > moitie_y){
	      send_response(inter.bix, inter.bsx, status.MPI_SOURCE);
	      send_response(inter.biy, moitie_y, status.MPI_SOURCE);
	      /*le nouveau noeud sera dans [biy ; moitie]*/
	    }
	    else{
	      send_response(inter.bix, inter.bsx, status.MPI_SOURCE);
	      send_response(moitie_y, inter.bsy, status.MPI_SOURCE);
	      /*le nouveau noeud sera dans [moitie ; bsy]*/
	    }
	
	  }
	  break;
	  /*dire voisin de l'ancien qu'il est le nouveau*/
    
	default:
	  break;
	}

	return 0;
  }

int main(int argc, char const *argv[])
{
  int nb_proc,*my_rank, bootstrap;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

  if(my_rank == 0){
    init_coordinateur(my_rank);
    coordinateur(my_rank);
  }
  else {
    init_node(my_rank);
    ask_insertion(my_rank);
  }


  MPI_Finalize();
  return 0;
}
