#include "../include/main.h"


void coordinateur(int my_rank){
  int msg[2];
  MPI_Status status;
  int cpt_nodes = 1;
  printf("%d : coordinateur\n", my_rank);
  while(cpt_nodes<N){
    MPI_Recv(msg, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    switch (status.MPI_TAG){
    case __TAG_NODE_INSERTED:
      printf("%d : received TAG__NODE_INSERTED\n", my_rank);
      cpt_nodes++;
      MPI_Send(&id_coord,3, MPI_INT, cpt_nodes, __TAG_BEGIN_INSERT, MPI_COMM_WORLD);
      break;

    default: 
      break;
    }
  }
}

int ask_insertion(int my_rank){
  int msg, msg_coord[3], buf[8];
  MPI_Status status;
  int dif_borne_x, dif_borne_y,  moitie_x, moitie_y, done=0, dest;
  nei_list *list_up = NULL, *list_down = NULL, *list_left = NULL, *list_right = NULL; 

  while(1){

    MPI_Recv(msg_coord, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch(status.MPI_TAG){


    case __TAG_BEGIN_INSERT:
      printf("%d : received __TAG_BEGIN_INSERT %d\n", my_rank, status.MPI_TAG);
      if (my_rank>1){
        /* all the nodes ask where to insert themself */
        msg_coord[0]=id_coord.x;
        msg_coord[1]=id_coord.y;
        msg_coord[2]=my_rank;
        MPI_Send(&msg_coord,3, MPI_INT, 1, __TAG_INSERT_ME, MPI_COMM_WORLD);
        printf("%d : envoi à 1 de __TAG_INSERT_ME\n", my_rank); 
      }
      else{
        /* the first node is imediately inserted in the canvas */
        MPI_Send(msg_coord, 3, MPI_INT, 0, __TAG_NODE_INSERTED, MPI_COMM_WORLD); 
      }
      break;


    case __TAG_INSERT_ME:
  	  printf("%d : received __TAG_INSERT_ME received : %d %d , bornes : %d %d %d %d\n", my_rank, msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.bsy);
      if((inter.bix < msg_coord[0]) && (msg_coord[0] < inter.bsx)){
        if((inter.biy < msg_coord[1]) && (msg_coord[1] < inter.bsy)){
      	  dif_borne_x = dif(inter.bsx, inter.bix);
      	  dif_borne_y = dif(inter.bsy, inter.biy);
    	    moitie_x = dif_borne_x/2;
          moitie_y = dif_borne_y/2;







          if(dif_borne_x > dif_borne_y){
            printf("%d : my zone is a rectangle horizontal\n", my_rank);
      	    if (id_coord.x > moitie_x){
      	      /*le nouveau noeud sera dans [bix ; moitie]*/

              printf("%d : [LEFT]bornes we send to node : %d %d %d %d\n", my_rank, inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);


              send_response(inter.bix, inter.bix+moitie_x, msg_coord[2]);
              send_response(inter.biy, inter.bsy, msg_coord[2]);


              /* add the node into my neighbours list */
              if(list_left == NULL){
                list_left = init_list(msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
                printf("%d : init list\n", my_rank);
              }
              else{
                add_to_queue(list_left, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
              }
              update_bornes(my_rank, inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy);
      	    }
      	    else{
      	      /*le nouveau noeud sera dans [moitie ; bsx]*/
              printf("%d : [RIGHT]bornes we send to node : %d %d %d %d\n", my_rank, moitie_x+inter.bix, inter.bsx, inter.biy, inter.bsy);
              send_response(inter.bix+moitie_x, inter.bsx, msg_coord[2]);
              send_response(inter.biy, inter.bsy, msg_coord[2]);
              if(list_right == NULL){
                list_right = init_list(msg_coord[2], msg_coord[0], msg_coord[1], moitie_x+inter.bix, inter.bsx, inter.biy, inter.bsy);
                printf("%d : init list\n", my_rank);
              }
              else{
                add_to_queue(list_right, msg_coord[2], msg_coord[0], msg_coord[1], moitie_x+inter.bix, inter.bsx, inter.biy, inter.bsy);
              }
              update_bornes(my_rank, inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
      	    }
          }




      	  else if (dif_borne_x < dif_borne_y){
      	    printf("%d : my zone is a rectangle vertical\n", my_rank);
            if (id_coord.y > moitie_y){
      	      /*le nouveau noeud sera dans [biy ; moitie]*/

              printf("%d : [DOWN]bornes we send to node : %d %d %d %d\n", my_rank, inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y);
              send_response(inter.bix, inter.bsx, msg_coord[2]);
              send_response(inter.biy, inter.biy+moitie_y, msg_coord[2]);
              if(list_down == NULL){
                list_down = init_list(msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y);
                printf("%d : init list\n", my_rank);

              }
              else{
                add_to_queue(list_down, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y);
              }
              update_bornes(my_rank, inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy);

      	    }
      	    else{
      	      /*le nouveau noeud sera dans [moitie ; bsy]*/

              printf("%d : [UP]bornes we send to node : %d %d %d %d\n", my_rank, inter.bix, inter.bsx, moitie_y, inter.bsy);
              send_response(inter.bix, inter.bsx, msg_coord[2]);
              send_response(inter.biy+moitie_y, inter.bsy, msg_coord[2]);
              if(list_up == NULL){
                list_up = init_list(msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy);
                printf("%d : init list\n", my_rank);
              }
              else{
                add_to_queue(list_up, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy);
              }
              update_bornes(my_rank, inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y);

      	    }
      	  }







          else{
            printf("%d : my zone is a carre\n", my_rank);
            if (id_coord.x > moitie_x){
              /*if in the square i am in more than the half on x we make 2 vertical rectangles, choice to make */
              printf("%d : [LEFT]bornes we send to node : %d %d %d %d\n", my_rank, inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);


              send_response(inter.bix, inter.bix+moitie_x, msg_coord[2]);
              send_response(inter.biy, inter.bsy, msg_coord[2]);

              if(list_left == NULL){
                printf("%d %d %d %d \n", inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
                list_left = init_list(msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
                printf("%d : init list %p\n", my_rank, list_left);
              }
              else{
                add_to_queue(list_left, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
              }
              
              update_bornes(my_rank, inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy);

            }
            else if(id_coord.y > moitie_y){
              /*if in the square i am in more than the half of the size on y we make 2 horizontal rectangles, choice to make*/
              printf("%d : [DOWN]bornes we send to node : %d %d %d %d\n", my_rank, inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y);
              send_response(inter.bix, inter.bsx, msg_coord[2]);
              send_response(inter.biy, inter.biy+moitie_y, msg_coord[2]);
              if(list_down == NULL){
                list_down = init_list(msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y);
                printf("%d : init list %p\n", my_rank, list_down);
              }
              else{
                add_to_queue(list_down, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y);
              }
              update_bornes(my_rank, inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy);

            }
            else{
              /* we are less than the half on x and on y (we're at the bottom left) */
              /* random choice, we can in horizontal or in vertical rectangles*/
              /* here the choice is to make vertical rectangles and, obviously, stay on the left */
              printf("%d : [RIGHT]bornes we send to node : %d %d %d %d\n", my_rank, inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy);
              send_response(inter.bix+moitie_x, inter.bsx, msg_coord[2]);
              send_response(inter.biy, inter.bsy, msg_coord[2]);
              if(list_right == NULL){
                list_right = init_list(msg_coord[2], msg_coord[0], msg_coord[1], inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy);
                printf("%d : init list %p\n", my_rank, list_right);
              }
              else{
                add_to_queue(list_right, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy);
              }
              update_bornes(my_rank, inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
            }
          }
        }
      }
      else{
        /*route node to the good neighbour*/
        printf("%d : route to neighbours\n", my_rank);
        if(msg_coord[0] > inter.bsx){
          /* send to right neighbour */
          dest = find_neighbour(list_right, msg_coord[1], 1);
          printf("%d : found %d on right\n", my_rank, dest);
          MPI_Send(msg_coord, 3, MPI_INT, dest, __TAG_INSERT_ME, MPI_COMM_WORLD);
        }
        else if(msg_coord[0] < inter.bix){
          /* send to left neghbour */
          dest = find_neighbour(list_left, msg_coord[1], 1);
          printf("%d : found %d on left\n", my_rank, dest);
          MPI_Send(msg_coord, 3, MPI_INT, dest, __TAG_INSERT_ME, MPI_COMM_WORLD);

        }
        else if(msg_coord[1] > inter.bsy){
          /* send to up neighbour */
          dest = find_neighbour(list_up, msg_coord[0], 0);
          printf("%d : found %d on up\n", my_rank, dest);
          MPI_Send(msg_coord, 3, MPI_INT, dest, __TAG_INSERT_ME, MPI_COMM_WORLD);
        }
        else{
          /* send to down neighbour */
          dest = find_neighbour(list_down, msg_coord[0], 0);
          printf("%d : found %d on down\n", my_rank, dest);
          MPI_Send(msg_coord, 3, MPI_INT, dest, __TAG_INSERT_ME, MPI_COMM_WORLD);
        }
      }
	    break;


    case __TAG_RESP_INSERT:
      if (done){
        printf("%d : received __TAG_RESP_INSERT %d\n", my_rank, status.MPI_TAG);
        if (is_in(id_coord.x, buf[0], buf[1]) && is_in(id_coord.y, msg_coord[0], msg_coord[1]) ){
          //printf("%d : in my bornes\n", my_rank);
          update_bornes(my_rank, buf[0], buf[1], msg_coord[0], msg_coord[1]);
        }
        else{
          choose_new_coords(my_rank, buf[0], buf[1], msg_coord[0], msg_coord[1]);
        }
        MPI_Send(msg_coord, 3, MPI_INT, 0, __TAG_NODE_INSERTED, MPI_COMM_WORLD);
        done=0;
      }
      else{
        buf[0]=msg_coord[0];
        buf[1]=msg_coord[1];
        done=1;
      }


    case __TAG_ADD_NEIGH:
      done=0;
      if(done < 8){
        buf[done]=msg_coord[0];
        if(done > 1)
          buf[done+1] = msg_coord[1];
        done+=2;
      }
      else{

        if(buf[2] > inter.bsx){
          /* save to left neighbour */
          if(list_left == NULL){
            list_left = init_list(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
          }
          else{
            add_to_queue(list_left, buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
          }
        }
        else if(buf[2] < inter.bix){
          /* save to right neghbour */
          if(list_right == NULL){
            list_right = init_list(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
          }
          else{
            add_to_queue(list_right, buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
          }

        }
        else if(buf[3] > inter.bsy){
          /* save to up neighbour */
          if(list_up == NULL){
            list_up = init_list(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
          }
          else{
            add_to_queue(list_up, buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
          }

        }
        else{
          /* save to down neighbour */
          if(list_down == NULL){
            list_down = init_list(buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
          }
          else{
            add_to_queue(list_down, buf[0], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
          }
        }
        done=0;
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
