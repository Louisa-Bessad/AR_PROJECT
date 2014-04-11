#include "../include/main.h"


void coordinateur(int my_rank){
  int msg[3];
  MPI_Status status;
  int cpt_nodes = 2;
  printf("%d : coordinateur\n", my_rank);
  while(cpt_nodes<=N+1){
    MPI_Recv(msg, 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    switch (status.MPI_TAG){
    case __TAG_NODE_INSERTED:
      /* switching to the next node to insert */
      /* coordinator sends a message to engage the node's insertion */
      printf("%d : received TAG__NODE_INSERTED\n", my_rank);
      if(cpt_nodes < N+1){
        printf("%d : sending __TAG_BEGIN_INSERT to %d\n", my_rank, cpt_nodes);
        MPI_Send(&msg,3, MPI_INT, cpt_nodes, __TAG_BEGIN_INSERT, MPI_COMM_WORLD);
      }
      cpt_nodes++;
      break;

    default: 
      break;
    }
  }
  printf("%d : out\n", my_rank);
}

int ask_insertion(int my_rank){
  int msg, msg_coord[3], buf[8];
  MPI_Status status;
  int dif_borne_x, dif_borne_y,  moitie_x, moitie_y, done=0, dest;
  nei_list *list_up = NULL, *list_down = NULL, *list_left = NULL, *list_right = NULL, *tosend = NULL; 

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

        /* Sending a request for insertion to the BOOTSTRAP*/
        MPI_Send(&msg_coord,3, MPI_INT, 1, __TAG_INSERT_ME, MPI_COMM_WORLD);
        printf("%d : envoi à 1 de __TAG_INSERT_ME\n", my_rank); 
      }
      else{
        /* the first node is imediately inserted in the canvas */
        MPI_Send(msg_coord, 3, MPI_INT, 0, __TAG_NODE_INSERTED, MPI_COMM_WORLD); 
      }
      break;










    case __TAG_INSERT_ME:
  	  printf("%d : received __TAG_INSERT_ME received : %d %d , my bornes : %d %d %d %d\n", my_rank, msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.bsy);
      
      if(is_in(msg_coord[0], inter.bix, inter.bsx) && is_in(msg_coord[1], inter.biy, inter.bsy)){

        /******* The neighbour is trying to insert into the area we "control" ******/
    	  dif_borne_x = dif(inter.bsx, inter.bix);
    	  dif_borne_y = dif(inter.bsy, inter.biy);
  	    moitie_x = dif_borne_x/2;
        moitie_y = dif_borne_y/2;


        if(dif_borne_x > dif_borne_y){

          printf("%d : my zone is a rectangle horizontal\n", my_rank);
    	    if (id_coord.x > moitie_x){
    	      /*the new node will be in [bix ; moitie]*/
            printf("%d : [LEFT]bornes we send to node : %d %d %d %d\n", my_rank, inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);

            /* We send him the area he will manage */
            send_response(inter.bix, inter.bix+moitie_x, msg_coord[2]);
            send_response(inter.biy, inter.bsy, msg_coord[2]);


            /* add the node into my neighbours list */
            if(list_left == NULL){
              list_left = init_list(msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
            }
            else{
              /* add the new neighbour to the correspondigng line */
              add_to_queue(list_left, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);

              /* telling all neighbours on this side that they'll need to add the new guy */
              update_all_neighbours(list_left, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy, 3);

              /* we tell them that we are not anymore neighbours... Sorry :'( maybe one day */
              send_del_to_neighbours(list_left,my_rank, 3);
            }

            /********* Now , since we split in two parts, some of our neighbours will still be in our area, but some not. So we need to update our value in their list
            when we are still neighbours, and the others will have our new neighbour as a neighbour

            **** for example, in this case, if  we split vertically, our UP and DOWN side are concerned by this statement *****/


            if(list_up != NULL){
            /* Seraching neighbours that will ONLY be aside THE NEW NODE*/
            tosend = find_neighbours_to_update(list_up, inter.bix, inter.bix+moitie_x, 0, 4);

            /* we send them the new guy */
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,4);

            /* we tell them that... it is over for us... :'''( we had good times together */
            send_del_to_neighbours(tosend,my_rank,4);

            /* finding neighbours that will only be aside US */
            tosend = find_neighbours_to_update(list_up, inter.bix+moitie_x, inter.bsx, 0, 4);

            /* updating our value in their list */
            update_me_all_neighbours(tosend, my_rank, inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy,4);
            }
            if(list_down != NULL){
            tosend = find_neighbours_to_update(list_down, inter.bix, inter.bix+moitie_x, 0, 2);
            /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,2);
            send_del_to_neighbours(tosend,my_rank,2);
            tosend = find_neighbours_to_update(list_down, inter.bix+moitie_x, inter.bsx, 0, 2);
            update_me_all_neighbours(tosend, my_rank, inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy,2);
            }

            /*we send to neighbours that we change our bournes*/

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
              update_all_neighbours(list_right, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy, 1);
              send_del_to_neighbours(list_right,my_rank,1);
            }

            if(list_up != NULL){
              tosend = find_neighbours_to_update(list_up, moitie_x+inter.bix, inter.bsx, 0, 4);
             /*We search neighbours to update*/
              send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], moitie_x+inter.bix, inter.bsx, inter.biy, inter.bsy,4);
              send_del_to_neighbours(tosend,my_rank,4);
              tosend = find_neighbours_to_update(list_up, inter.bix+moitie_x, inter.bsx, 0, 4);
              update_me_all_neighbours(tosend, my_rank, inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,4);
            }
            if(list_down != NULL){
              tosend = find_neighbours_to_update(list_down, moitie_x+inter.bix, inter.bsx, 0, 2);
             /*We search neighbours to update*/
              send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], moitie_x+inter.bix, inter.bsx, inter.biy, inter.bsy,2);
              send_del_to_neighbours(tosend,my_rank,2);
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
              update_all_neighbours(list_down, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y, 2);

              send_del_to_neighbours(list_down,my_rank,2);
            }

          if(list_right != NULL){
            tosend = find_neighbours_to_update(list_right, inter.biy, inter.biy+moitie_y, 1,1);
           /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y,1);
            send_del_to_neighbours(tosend,my_rank,1);
            tosend = find_neighbours_to_update(list_right, inter.bix+moitie_x, inter.bsx, 0,1);
            update_me_all_neighbours(tosend, my_rank, inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy,1);
          }
  
        if(list_left!=NULL){      
            tosend = find_neighbours_to_update(list_left, inter.biy, inter.biy+moitie_y, 1,3);
            /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y,3);
            send_del_to_neighbours(tosend,my_rank,3);
            tosend = find_neighbours_to_update(list_left, inter.bix+moitie_x, inter.bsx, 0,3);
            update_me_all_neighbours(tosend, my_rank, inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy,3);

        }
            /*we send to neighbours that we change our bournes*/


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
              update_all_neighbours(list_up, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy, 4);
              send_del_to_neighbours(list_up,my_rank,4);
            } 

            if (list_right!=NULL){
              tosend = find_neighbours_to_update(list_right, inter.biy, inter.biy+moitie_y, 1,1);
             /*We search neighbours to update*/
              send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy,1);
              send_del_to_neighbours(tosend,my_rank,1);
              tosend = find_neighbours_to_update(list_right, inter.bix+moitie_x, inter.bsx, 0,1);
              update_me_all_neighbours(tosend, my_rank, inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y,1);
            }
            if(list_left!=NULL){
              tosend = find_neighbours_to_update(list_left, inter.biy, inter.biy+moitie_y, 1,3);
              /*We search neighbours to update*/
              send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy,3);
              send_del_to_neighbours(tosend,my_rank,3);
              tosend = find_neighbours_to_update(list_left, inter.bix+moitie_x, inter.bsx, 0,3);
              update_me_all_neighbours(tosend, my_rank, inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y,3);
            }
            /*we send to neighbours that we change our bournes*/


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
             update_all_neighbours(list_left, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,3);
            send_del_to_neighbours(list_left,my_rank,3);
            }

            if(list_up!=NULL){
            tosend = find_neighbours_to_update(list_up, inter.bix, inter.bix+moitie_x, 0,4);
            /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,4);
            send_del_to_neighbours(tosend,my_rank,4);
            tosend = find_neighbours_to_update(list_up, inter.bix+moitie_x, inter.bsx, 0,4);
            update_me_all_neighbours(tosend, my_rank, inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy,4);
          }

          if(list_down){
            tosend = find_neighbours_to_update(list_down, inter.bix, inter.bix+moitie_x, 0,2);
            /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,2);
            send_del_to_neighbours(tosend,my_rank,2);
            tosend = find_neighbours_to_update(list_down, inter.bix+moitie_x, inter.bsx, 0,2);
            update_me_all_neighbours(tosend, my_rank, inter.bix+moitie_x, inter.bsx, inter.biy, inter.bsy,2);
          }
          
            /*we send to neighbours that we change our bournes*/


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
              update_all_neighbours(list_down, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y,2);
              send_del_to_neighbours(list_down,my_rank,2);
            }            

            if(list_right!=NULL){
            tosend = find_neighbours_to_update(list_right, inter.biy, inter.biy+moitie_y, 1,1);
            /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y,1);
            send_del_to_neighbours(tosend,my_rank,1);
            tosend = find_neighbours_to_update(list_right, inter.bix+moitie_x, inter.bsx, 0,1);
            update_me_all_neighbours(tosend, my_rank, inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy,1);
          }

          if(list_left){
            tosend = find_neighbours_to_update(list_left, inter.biy, inter.biy+moitie_y, 1,3);
            /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bsx, inter.biy, inter.biy+moitie_y,3);
            send_del_to_neighbours(tosend,my_rank,3);
            tosend = find_neighbours_to_update(list_left, inter.bix+moitie_x, inter.bsx, 0,3);
            update_me_all_neighbours(tosend, my_rank,  inter.bix, inter.bsx, inter.biy+moitie_y, inter.bsy,3);
          }
            /*we send to neighbours that we change our bournes*/

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
              update_all_neighbours(list_right, msg_coord[2], msg_coord[0], msg_coord[1], inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,1);
              send_del_to_neighbours(list_right,my_rank,1);
            }            


            if(list_up!=NULL){
            tosend = find_neighbours_to_update(list_up, moitie_x+inter.bix, inter.bsx, 0,4);
            /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], moitie_x+inter.bix, inter.bsx, inter.biy, inter.bsy,4);
            send_del_to_neighbours(tosend,my_rank,4);
            tosend = find_neighbours_to_update(list_up, inter.bix+moitie_x, inter.bsx, 0,4);
            update_me_all_neighbours(tosend, my_rank, inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,4);
          }

            if(list_down!=NULL){
            tosend = find_neighbours_to_update(list_down, moitie_x+inter.bix, inter.bsx, 0,2);
            /*We search neighbours to update*/
            send_add_to_neighbours(tosend, msg_coord[2], msg_coord[0], msg_coord[1], moitie_x+inter.bix, inter.bsx, inter.biy, inter.bsy,2);
            send_del_to_neighbours(tosend,my_rank,2);
            tosend = find_neighbours_to_update(list_down, inter.bix+moitie_x, inter.bsx, 0,2);
            update_me_all_neighbours(tosend, my_rank,  inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy,2);
          }
            /*we send to neighbours that we change our bournes*/


            update_bornes(my_rank, inter.bix, inter.bix+moitie_x, inter.biy, inter.bsy);
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
        if (!(is_in(id_coord.x, buf[0], buf[1]) && is_in(id_coord.y, msg_coord[0], msg_coord[1]))){

          /* we are not in the area the node containing our coords is going to insert us, so we change ou coords to match the good bornes */
          choose_new_coords(my_rank, buf[0], buf[1], msg_coord[0], msg_coord[1]);
        }

        update_bornes(my_rank, buf[0], buf[1], msg_coord[0], msg_coord[1]);
        MPI_Send(msg_coord, 3, MPI_INT, 0, __TAG_NODE_INSERTED, MPI_COMM_WORLD);
        done=0;
      }
      else{
        buf[0]=msg_coord[0];
        buf[1]=msg_coord[1];
        done=1;
      }
      break;







    case __TAG_ADD_NEIGH:
      done=0;
      if(done < 8){
        buf[done]=msg_coord[0];
        if(done > 0)
          buf[done+1] = msg_coord[1];
        done+=2;
      }
      else{

        /*Somebody's telling us to insert a new neighbour in our neighbours' list */

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
      break;






    case __TAG_DEL_NEIGH :
    /* somebody asks us to delete a neighbour */
      switch (buf[1]){
        case 1:
          del_neighbour_in_list(list_left, buf[0]);
      
      break;
        case 2:
          del_neighbour_in_list(list_up, buf[0]);
      
      break;
        case 3:
          del_neighbour_in_list(list_right, buf[0]);
      
      break;
        case 4:
          del_neighbour_in_list(list_down, buf[0]);
      
      break;
    }
    break;






    case __TAG_UPDATE_NEIGH :
      done=0;
      if(done < 8){
        buf[done]=msg_coord[0];
        if(done > 0)
          buf[done+1] = msg_coord[1];
        done+=2;
      }

      switch (buf[1]){
        case 1:
          update_node(list_left, buf[0], buf[4], buf[5], buf[6], buf[7]);
          break;
        case 2:
          update_node(list_up, buf[0], buf[4], buf[5], buf[6], buf[7]);
          break;
        case 3:
          update_node(list_right, buf[0], buf[4], buf[5], buf[6], buf[7]);
          break;
        case 4:
          update_node(list_down, buf[0], buf[4], buf[5], buf[6], buf[7]);
          break;
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
