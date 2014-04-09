#ifndef __MPI_PROJ_STRUCT
#define __MPI_PROJ_STRUCT

typedef struct coord{
	int x;
	int y;
} coordonnee;

typedef struct data{
	coordonnee id;
	int values;
} donnee;

typedef struct borne{
  int bix;
  int bsx;
  int biy;
  int bsy;
} borne;


typedef struct double_circular_linked_list{
	int rank;
	coordonnee point;
	borne inter;
	struct double_circular_linked_list *next;
	struct double_circular_linked_list *prev;
} nei_list;

#endif
