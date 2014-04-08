#include "../include/tools.h"

int dif(int val1, int val2){
  return val1-val2;
}

int send_response(int val1, int val2, int dest){
  int msg_coord[2];
  msg_coord[0] = val1;
  msg_coord[1] = val2;
  MPI_Send(msg_coord, 2, MPI_INT, dest, __TAG_RESP_INSERT, MPI_COMM_WORLD);
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
	id_coord.y = ((sup_y-inf_y)*rand())+inf_y;;
	printf("%d : choosed new coords matching %d %d %d %d : %d %d  \n", my_rank, inf_x, sup_x, inf_y, sup_y, id_coord.x, id_coord.y);
	return 1;
}