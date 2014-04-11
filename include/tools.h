#ifndef __MPI_PROJ_TOOLS
#define __MPI_PROJ_TOOLS


#include "struct.h"
#include "define.h"
#include "main.h"



int send_response(int, int, int);
int dif(int, int);
int update_bornes(int, int, int, int, int);
int is_in(int, int, int);
int choose_new_coords(int, int, int, int, int);
void add_before (nei_list*, int, int, int, int, int, int, int);
void add_after (nei_list*, int, int, int, int, int, int, int);
void add_to_queue (nei_list*, int, int, int, int, int, int, int);
int find_neighbour (nei_list*, int, int);


nei_list* find_neighbours_to_update(nei_list* , int, int, int, int);
int update_all_neighbours(nei_list*, int, int, int, int, int, int, int,int);
void send_add_to_neighbours(nei_list*, int, int, int, int, int, int, int, int);
void send_del_to_neighbours(nei_list*, int, int);
int send_update(int, int, int);
void update_me_all_neighbours(nei_list*, int, int, int, int, int, int);
void update_node (nei_list*, int , int , int , int , int );
void del_neighbour_in_list(nei_list*, int );

#endif