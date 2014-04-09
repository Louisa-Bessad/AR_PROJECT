#ifndef __MPI_PROJ_INIT
#define __MPI_PROJ_INIT

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include "struct.h"
#include "define.h"


coordonnee id_coord;
coordonnee bootstrap;
borne inter;
 
int init_coordinateur(int);
int init_node(int);
coordonnee init_struct(int, int);
borne init_borne( int, int, int, int);
nei_list* init_list (int , int , int , int , int , int , int );

#endif
