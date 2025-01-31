
/*
 * File:     treesum_test_mpi.c
 *
 * Purpose:  test tree-structured communication to find the global sum
 *           of a random collection of ints.  This version doesn't
 *           require that no of processes  be a power of 2.
 *
 * Compile:  mpicc -Wall -o treesum_test_mpi  treesum_test_mpi.c treesum_mpi.c
 * minunit.c Run:      ibrun  -v ./treesum_test_mpi
 *
 * IPP:      Programming Assignment Text Book Problem 3.3 Page 148
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

int my_rank, no_proc;
MPI_Comm comm;

int global_sum(int my_int, int my_rank, int no_proc, MPI_Comm comm);

/*-------------------------------------------------------------------
 * Test global_sum()
 * If successful, return NULL
 */
char *treesum_test1() {
  /* Your solution */

  // allocate memory and make rand list of int
  int *local_list = malloc(no_proc * sizeof(int));
  int all_succ;
  int succ = local_list != NULL;
  MPI_Allreduce(&succ, &all_succ, 1, MPI_INT, MPI_PROD, comm);
  if (all_succ == 0) return "Memory allocation failed";

  int my_int = rand() % 10;

  MPI_Allgather(&my_int, 1, MPI_INT, local_list, 1, MPI_INT, comm);

  // run prog in parallel
  double startwtime = 0, endwtime = 0;
  if (my_rank == 0) startwtime = MPI_Wtime();
  int func_sum = global_sum(my_int, my_rank, no_proc, comm);
  if (my_rank == 0) {
    endwtime = MPI_Wtime();
    printf("Test 1: Wall clock time = %f at Proc 0 of %d processes\n",
           endwtime - startwtime, no_proc);
    int real_sum = 0;
    for(int i = 0; i < no_proc; i++) real_sum += local_list[i];
    free(local_list);
    if(func_sum != real_sum) return "Incorrect sum";
  } else {
    free(local_list);
  }

  return NULL;
}

/*-------------------------------------------------------------------
 * Run all tests.  Ignore returned messages.
 */
void run_all_tests(void) { 
  mu_run_test(treesum_test1);
}

/*-------------------------------------------------------------------
 * The main entrance to run all tests.
 * Only Proc 0 prints the test summary
 */
int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &no_proc);
  MPI_Comm_rank(comm, &my_rank);

  run_all_tests();

  if (my_rank == 0) {
    mu_print_test_summary("Summary:");
  }
  MPI_Finalize();
  return 0;
}
