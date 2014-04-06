#include "main.h"
#include "init.h"
#include  "main.h"

void coordinateur(int my_rank){
	bootstrap.x = /* a recuperer dans msg*/;
	bootstrap.y = /* a recuperer dans msg*/;

}

int ask_insertion(int my_rank){
	int msg, msg_coor[2];
	MPI_Status status;

	MPI_Recv(&msg, 1, MPI_INT, 0, ANY_TAG, MPI_COMM_WORLD, &status); /* Ok pour s'insérer (recu du coordinateur)*/
	MPI_Send(&msg_coor,2, MPI_INT,bootstrap,ANY_TAG, MPI_COMM_WORLD); /* Demande et envoie des coordonnees au bootstrap*/
	MPI_Recv(&msg, 1, MPI_INT, ANY_SOURCE, ANY_TAG, MPI_COMM_WORLD, &status); /* Attente de l'accord de la zone ou il doit s'inserer*/
	/*Modifier ses bornes*/
	/*dire voisin de l'ancien qu'il est le nouveau*/
	

	return 0;
}


int main(int argc, char const *argv[])
{
	int nb_proc,*my_rank, bootstrap;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	if(my_rank == 0){
		init_coordinateur();
		coordinateur();
	}
	else {
		init_node();
		ask_insertion();
	}


	MPI_Finalize();
	return 0;
}