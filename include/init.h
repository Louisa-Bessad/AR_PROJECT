#ifndef __MPI_PROJ_INIT
#define __MPI_PROJ_INIT

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "struct.h"
#include "define.h"


coordonnee id_coord;
coordonnee bootstrap;
borne inter;
 
int init_coordinateur(int);
int init_node(int);
void init_struct(int, int, coordonnee *);

#endif
