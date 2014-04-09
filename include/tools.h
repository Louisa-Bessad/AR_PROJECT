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

#endif