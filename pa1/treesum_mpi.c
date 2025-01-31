/*
 * File:     treesum_mpi.c
 *
 * Purpose:  Use tree-structured communication to find the global sum
 *           of a random collection of ints.  This version doesn't
 *           require that comm_sz be a power of 2.
 *
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/*-------------------------------------------------------------------
 * Function:
 *  global_sum
 *
 * Purpose:
 *  Implement a global sum using tree-structured communication
 *
 * Notes:
 *  1.  The return value for global sum is only valid on process 0
 */
int global_sum(int my_int /* in */, int my_rank /* in */, int comm_sz /* in */,
               MPI_Comm comm /* in */) {
  int my_sum = my_int;

  /* Your solution */
  int lrg_power_two = 1;
  while (lrg_power_two*2 <= comm_sz)
    lrg_power_two *= 2;
  MPI_Status status;
  
  // make init tree size a power of 2
  if(my_rank >= lrg_power_two && my_rank < comm_sz) {
    MPI_Send(&my_sum, 1, MPI_INT, my_rank - lrg_power_two, 0, comm);
  } else if (my_rank < comm_sz - lrg_power_two) {
    int val;
    MPI_Recv(&val, 1, MPI_INT, my_rank + lrg_power_two, 0, comm, &status);
    my_sum += val;
  }

  if(my_rank >= lrg_power_two) {
    return my_sum;
  }

  // tree sum algo
  if((my_rank % 2 != 0 && my_rank > 0))
    MPI_Send(&my_sum, 1, MPI_INT, my_rank - 1, 0, comm);
  for(int gap=1; gap<lrg_power_two; gap *= 2) {
    if(my_rank % (2 * gap) == 0) {
      int other_sum;
      MPI_Recv(&other_sum, 1, MPI_INT, my_rank + gap, 0, comm, &status);
      my_sum = other_sum + my_sum;
      if((my_rank % (4 * gap) != 0 && my_rank >= 2 * gap)) {
        MPI_Send(&my_sum, 1, MPI_INT, my_rank - 2*gap, 0, comm);
      }
    }
  }

  return my_sum;
} /* Global_sum */
