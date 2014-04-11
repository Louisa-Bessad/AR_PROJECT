#ifndef __MPI_PROJ
#define __MPI_PROJ


/* The main header of the program, containing main functions */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>
#include "struct.h"
#include "define.h"
#include "tools.h"
#include "init.h"


void coordinateur(int);
int ask_insertion(int);

#endif
