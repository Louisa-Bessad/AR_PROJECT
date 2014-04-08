#include "../include/main.h"


void coordinateur(int my_rank){
  int msg[2];
  MPI_Status status;
  int cpt_nodes = 1;
  printf("%d : coordinateur\n", my_rank);
  while(cpt_nodes<N){
    MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    switch (status.MPI_TAG){
    case __TAG_NODE_INSERTED:
      printf("%d : received TAG__NODE_INSERTED\n", my_rank);
      cpt_nodes++;
      MPI_Send(&id_coord,2, MPI_INT, cpt_nodes, __TAG_BEGIN_INSERT, MPI_COMM_WORLD);
      break;

    default: 
      break;
    }
  }
}

int ask_insertion(int my_rank){
  int msg, msg_coord[2], buf[2];
  MPI_Status status;
  int dif_borne_x, dif_borne_y,  moitie_x, moitie_y, done=0;
  
  while(1){

    MPI_Recv(msg_coord, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch(status.MPI_TAG){


    case __TAG_BEGIN_INSERT:
      printf("%d : received %d\n", my_rank, status.MPI_TAG);
      if (my_rank>1){
        MPI_Send(&id_coord,2, MPI_INT, 1, __TAG_INSERT_ME, MPI_COMM_WORLD);
        printf("%d : envoi à 1 de __TAG_INSERT_ME\n", my_rank); 
        //MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      }
      //printf("%d : inserted\n", my_rank);
      MPI_Send(msg_coord, 2, MPI_INT, 0, __TAG_NODE_INSERTED, MPI_COMM_WORLD);
      break;


    case __TAG_INSERT_ME:
      printf("%d : received %d\n", my_rank, status.MPI_TAG);
      if((inter.bix < msg_coord[0]) && (msg_coord[0] < inter.bsx)){
      	if((inter.biy < msg_coord[1]) && (msg_coord[1] < inter.bsy)){
      	  printf("%d : received INSERT_ME received : %d %d , bornes : %d %d %d %d\n", my_rank, msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.bsy);
      	  dif_borne_x = dif(inter.bsx, inter.bix);
      	  dif_borne_y = dif(inter.bsy, inter.biy);
      	  if(dif_borne_x >= dif_borne_y){
      	    /*rectangle horizontal*/
      	    printf("rectangle horizontal ou carre");
      	    moitie_x = dif_borne_x/2;
      	    if (id_coord.x > moitie_x){
      	      /*le nouveau noeud sera dans [bix ; moitie]*/

              printf("%d : il est dans ma zone,  %d %d %d %d\n", my_rank, inter.bix, moitie_x, inter.biy, inter.bsy);
              send_response(inter.bix, moitie_x, status.MPI_SOURCE);
              send_response(inter.biy, inter.bsy, status.MPI_SOURCE);
              update_bornes(my_rank, moitie_x, inter.bix, inter.biy, inter.bsy);
      	    }
      	    else{
      	      /*le nouveau noeud sera dans [moitie ; bsx]*/

              printf("%d : il est dans ma zone,  %d %d %d %d\n", my_rank, moitie_x, inter.bsx, inter.biy, inter.bsy);
              send_response(moitie_x, inter.bsx, status.MPI_SOURCE);
              send_response(inter.biy, inter.bsy, status.MPI_SOURCE);
              update_bornes(my_rank, inter.bsx, moitie_x, inter.biy, inter.bsy);
      	    }
          }
      	  else{
      	    printf("rectangle vertical");
      	    /*rectangle vertical*/
      	    moitie_y = dif_borne_y/2;
            if (id_coord.y > moitie_y){
      	      /*le nouveau noeud sera dans [biy ; moitie]*/

              printf("%d : il est dans ma zone,  %d %d %d %d\n", my_rank, inter.bix, inter.bsx, inter.biy, moitie_y);
              send_response(inter.bix, inter.bsx, status.MPI_SOURCE);
              send_response(inter.biy, moitie_y, status.MPI_SOURCE);
              update_bornes(my_rank, inter.bix, inter.bsx, moitie_y, inter.biy);

      	    }
      	    else{
      	      /*le nouveau noeud sera dans [moitie ; bsy]*/

              printf("%d : il est dans ma zone,  %d %d %d %d\n", my_rank, inter.bix, inter.bsx, moitie_y, inter.bsy);
              send_response(inter.bix, inter.bsx, status.MPI_SOURCE);
              send_response(moitie_y, inter.bsy, status.MPI_SOURCE);
              update_bornes(my_rank, inter.bix, inter.bsx, inter.bsy, moitie_y);
      	    }
      	  }
        }
      }
      else{
        sleep(3);
      }
	    break;


    case __TAG_RESP_INSERT:
      if (done){
        if (is_in(id_coord.x, buf[0], buf[1]) && is_in(id_coord.y, msg_coord[0], msg_coord[1]) ){
          //printf("%d : in my bornes\n", my_rank);
          update_bornes(my_rank, buf[0], buf[1], msg_coord[0], msg_coord[1]);
        }
        else{
          choose_new_bornes(buf[0], buf[1], msg_coord[0], msg_coord[1]);
        }
      }
      else{
        buf[0]=msg_coord[0];
        buf[1]=msg_coord[1];
        done=1;
      }

    default:
	    break;
    }
  }
  return 0;
}


int main(int argc, char **argv){
  int nb_proc,my_rank, bootstrap;
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
