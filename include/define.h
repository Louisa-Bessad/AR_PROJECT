#ifndef __MPI_PROJ_DEFS
#define __MPI_PROJ_DEFS

#define N 5 // Number of nodes the coordinator will insert. Must be changed

#define __TAG_BEGIN_INSERT 1 // Coordinator allowing us to send a request to the bootstrap to insert us
#define __TAG_NODE_INSERTED 2 // a send to the bootstrap to tell him we have sucessfully been inserted into the canvas and he can skip to next node in his list
#define __TAG_INSERT_ME 3 // A request for any node to insert a new in the gived in the message 
#define __TAG_RESP_INSERT 4 // A respose from a node that is can insert in the area given in the message
#define __TAG_ADD_NEIGH 5 // A request to add the neighbour given in the message in his its corresponding list of neighbours
#define __TAG_DEL_NEIGH 6 // A request to delete the neighbour given in the message in his its corresponding list of neighbours 
#define __TAG_UPDATE_NEIGH 7 // A request to update the values of  the neighbour given in the message in his its corresponding list of neighbours 


#endif