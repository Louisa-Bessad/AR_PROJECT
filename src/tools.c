#include "../include/tools.h"

int dif(int val1, int val2){
  return val1-val2;
}

int send_response(int val1, int val2, int dest){
  int msg_coord[3];
  msg_coord[0] = val1;
  msg_coord[1] = val2;
  MPI_Send(msg_coord, 3, MPI_INT, dest, __TAG_RESP_INSERT, MPI_COMM_WORLD);
  return 1;
}

int update_bornes(int my_rank, int inf_x, int sup_x, int inf_y, int sup_y){

	inter.bix = inf_x;
	inter.bsx = sup_x;
	inter.biy = inf_y;
	inter.bsy = sup_y;
	printf("%d : updated my bornes : %d %d %d %d\n", my_rank, inf_x, sup_x, inf_y, sup_y);

	return 1;
}

int is_in(int val, int inf, int sup ){
	if((val > inf) && (val < sup)){
		return 1;
	}
	return 0;
}

int choose_new_coords(int my_rank, int inf_x, int sup_x, int inf_y, int sup_y ){

	id_coord.x = inf_x+(rand()%(sup_x-inf_x));
	id_coord.y = inf_y+(rand()%(sup_y-inf_y));
	printf("%d : choosed new coords matching %d %d %d %d : %d %d  \n", my_rank, inf_x, sup_x, inf_y, sup_y, id_coord.x, id_coord.y);
	return 1;
}

void add_before (nei_list* node, int rank, int coord_x, int coord_y, int bix, int bsx, int biy, int bsy){
  nei_list *nn = (nei_list*) malloc (sizeof(nei_list));
  if (nn != NULL){
  	nn->rank = rank;
    nn->point = init_struct(coord_x, coord_y);
    nn->inter = init_borne(bix, bsx, biy, bsy);

    nn->next = node;
    nn->prev = node->prev;
    
    node->prev->next = nn;
    node->prev = nn;
  }
}

void add_after (nei_list* node, int rank, int coord_x, int coord_y, int bix, int bsx, int biy, int bsy){
  nei_list *nn = (nei_list*) malloc (sizeof(nei_list));
  if (nn != NULL){
  	nn->rank = rank;
    nn->point = init_struct(coord_x, coord_y);
    nn->inter = init_borne(bix, bsx, biy, bsy);

    nn->prev = node;
    nn->next = node->next;
    
    node->next->prev = nn;
    node->next = nn;
  }
}

void add_to_queue (nei_list* racine, int rank, int coord_x, int coord_y, int bix, int bsx, int biy, int bsy){
  add_before(racine, rank, coord_x, coord_y, bix, bsx, biy, bsy);
}

int find_neighbour (nei_list* racine, int val, int mode){
	nei_list* it = racine;
	double first, end, res;
	if (mode){
		/* search on y */
		do
		{
			printf("\t mode %d val %d inter : %d ; %d\n", mode, val, it->inter.biy, it->inter.bsy);
			if((it->inter.biy <= val) && (val <= it->inter.bsy)){
				printf("\t racine : %p rank : %d\n", racine, it->rank);
				return it->rank;
			}
			it = it->next;
		} while (it != racine);
	}
	else{
		/* search on x */
		do
		{
			printf("\t racine : %p\n", racine);
			if((it->inter.bix < val) && (val < it->inter.bsx)){
				printf("\t\tici\n");
				return it->rank;
			}
			it = it->next;
			printf("\t mode %d val %d inter : %d ; %d\n", mode, val, inter.bix, inter.bsx);
		} while (it != racine);
	}
	return -1;
}
