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
				return it->rank;
			}
			it = it->next;
			printf("\t mode %d val %d inter : %d ; %d\n", mode, val, inter.bix, inter.bsx);
		} while (it != racine);
	}
	return -1;
}


int send_add(int val1, int val2, int dest){
  int msg_coord[3];
  msg_coord[0] = val1;
  msg_coord[1] = val2;
  MPI_Send(msg_coord, 3, MPI_INT, dest, __TAG_ADD_NEIGH, MPI_COMM_WORLD);
  return 1;
}

int send_del(int val1, int val2, int dest){
  int msg_coord[3];
  msg_coord[0] = val1;
  msg_coord[1] = val2;
  MPI_Send(msg_coord, 3, MPI_INT, dest, __TAG_DEL_NEIGH, MPI_COMM_WORLD);
  return 1;
}

int update_all_neighbours(nei_list* list, int rank, int coord_x, int coord_y, int inf_x, int sup_x, int inf_y, int sup_y, int val){

	nei_list* it_list = list;

	do {

		send_add(rank, val, it_list->rank);
		send_add(coord_x, coord_y, it_list->rank);
		send_add(inf_x, sup_x, it_list->rank);
		send_add(inf_y, sup_y, it_list->rank);

	} while(it_list->next != list);
} 

void update_me_all_neighbours(nei_list* list, int rank, int inf_x, int sup_x, int inf_y, int sup_y, int val){
	nei_list* it_list = list;

	do {

		send_update(rank, val, it_list->rank);
		send_update(id_coord.x, id_coord.y, it_list->rank);
		send_update(inf_x, sup_x, it_list->rank);
		send_update(inf_y, sup_y, it_list->rank);

	} while(it_list->next != list);
}


nei_list* find_neighbours_to_update(nei_list* list, int inf, int sup, int mode, int val){
	nei_list* it_list = list, *tosend = NULL;
	do
	{
		if (mode){

			if ((it_list->point.x < inf) && (sup < it_list->point.x)){
				if(tosend == NULL){
					tosend=it_list;
				}
				else{

					list -> prev -> next = tosend;
					list->prev = tosend;
					tosend -> prev = list->prev;
					tosend->next = list;

				}
			}

		}


		else{
			if (( it_list->point.y < inf) && (sup < it_list->point.y)){
				if(tosend == NULL){
					tosend=it_list;
				}
				else{
					list -> prev -> next = tosend;
					list->prev = tosend;
					tosend -> prev = list->prev;
					tosend->next = list;

				}
			}

		}

		it_list = it_list->next;
	}	while(it_list->next!=list);

	return tosend;
}


void send_add_to_neighbours(nei_list* list, int rank, int coord_x, int coord_y, int inf_x, int sup_x, int inf_y, int sup_y, int val){
	nei_list* it = list;
	do
	{
		send_add(rank, val, it->rank);
		send_add(coord_x, coord_y, it->rank);
		send_add(inf_x, sup_x, it->rank);
		send_add(inf_y, sup_y, it->rank);
	}while(it->next != list);
}



void send_del_to_neighbours(nei_list* list, int rank, int val){
	nei_list* it = list;
	do
	{
		send_del(rank, val, it->rank);
		
	}while(it->next != list);
}

int send_update(int val1, int val2, int dest){
  int msg_coord[3];
  msg_coord[0] = val1;
  msg_coord[1] = val2;
  MPI_Send(msg_coord, 3, MPI_INT, dest, __TAG_UPDATE_NEIGH, MPI_COMM_WORLD);
  return 1;
}


void del_neighbour_in_list(nei_list* list, int rank){
	nei_list* it = list;
	do
	{
		if(it->rank == rank){
			it->prev->next = it->next;
			it->next->prev = it-> prev;
			free(it);
			return;
		}
		
	}while(it->next != list);
}

void update_node (nei_list* list, int rank, int inf_x, int sup_x, int inf_y, int sup_y){
	nei_list* it = list;
	do
	{
		if(it->rank == rank){
			it->inter.bix = inf_x;
			it->inter.bsx = sup_x;
			it->inter.biy = inf_y;
			it->inter.bsy = sup_y;
			return;
		}
		
	}while(it->next != list);

}