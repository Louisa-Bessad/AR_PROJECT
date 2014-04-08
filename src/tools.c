#include "../include/tools.h"
#include "../include/main.h"
#include "../include/define.h"

int dif(int val1, int val2){
  return val1-val2;
}

int send_response(int val1, int val2, int dest){
  int msg_coord[2];
  msg_coord[0] = val1;
  msg_coord[1] = val2;
  return MPI_Send(msg_coord, 2, MPI_INT, dest, __TAG_RESP_INSERT, MPI_COMM_WORLD);
}
