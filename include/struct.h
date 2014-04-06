#ifndef __MPI_PROJ_STRUCT
#define __MPI_PROJ_STRUCT

typedef struct coord{
	int x;
	int y;
} coordonnee;

typedef struct do{
	coordonnee id;
	int values;
} donnee;

#endif