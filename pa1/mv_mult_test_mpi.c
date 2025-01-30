/*
 * File:
 *  mv_mult_test_mpi.c
 *
 * Purpose:
 *  test matrix vector multiplication y=Ax.
 *  Matrix A is a square matrix of size nxn.
 *  Column vectors x and y are of size nx1
 *
 * Input test matrix:
 *  A[i][j]=c in all positions.  y[i] is 0 in all positions x[i]= i
 *  for 0<=i<n. For simplicity, we assume n is divisible by no_proc.
 */

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

int my_rank, no_proc;
MPI_Comm comm;

/*Block mapping functions*/

#define procmap(i, r) ((int)floor((double)i / r))
#define local(i, r) (i % r)

int mat_vect_mult(double *, double *, double *, int, int, int, int, MPI_Comm);

/*-------------------------------------------------------------------
 * Test if the  matrix-vector multiplicatioon result is expected.
 * If failed, return a message string showing the failed point
 * If successful, return NULL
 *
 * In args:
 * 	y[] is the local array hosted at each process with block mapping
 * 	blocksize is the size of local array for vector y
 * 	n is the number of columns (and rows)
 * 	c is the constant value of all matrix elements in A
 */
char *validate_vect(double y[], int blocksize, int n, int c) {
  int i, local_i;
  double expected = 0.5 * c * n * (n - 1);

  for (i = 0; i < n; i++) {
    if (procmap(i, blocksize) == my_rank) {
      local_i = local(i, blocksize);
#ifdef DEBUG1
      printf("Proc %d: i=%d local_i=%d Expected %f Actual %f\n", my_rank, i,
             local_i, expected, y[local_i]);
#endif
      mu_assert("One mismatch in mat. vect. multiplication",
                y[local_i] == expected);
    }
  }
  return NULL;
}

/*-------------------------------------------------------------------
 * Allocate storage space for each array at each processs.
 * If failed, 0
 * If successful, return 1
 *
 * In args:
 *   	*local_A is the starting address of local space of matrix A
 *   	with rowwise block mapping;
 *   	*local_x is the starting address of local space of vector x
 *    with rowwise block mapping;
 *    *local_y is the starting address of local space of vector y
 *    with rowwise block mapping;
 *    blocksize is the size of local array n is the number of columns (and rows)
 */

int allocate_space(double **local_A, double **local_x, double **local_y,
                   int blocksize, int n) {
  int succ = 1, all_succ = 1;

  *local_A = malloc(blocksize * n * sizeof(double));
  *local_x = malloc(blocksize * sizeof(double));
  *local_y = malloc(blocksize * sizeof(double));
  /* Here we assume none of them are NULL. A more robust program needs to
     check the failed memory allocation */
  if (*local_A == NULL || *local_x == NULL || *local_y == NULL) {
    /* Find an error, thus we release space first */
    if (*local_A != NULL) free(*local_A);
    if (*local_x != NULL) free(*local_x);
    if (*local_y != NULL) free(*local_y);
    succ = 0;
  }
  /* All processes synchronize to check if there is any failure in allocation
   */
  MPI_Allreduce(&succ, &all_succ, 1, MPI_INT, MPI_PROD, comm);
  return all_succ;
}

/*-------------------------------------------------------------------
 * Test matrix vector multiplication
 * Process 0 collects the  error detection. If failed, return a message string
 * If successful, return NULL
 */

char *mv_test(char *testmsg, int n, int c) {
  double startwtime = 0, endwtime = 0;
  double *local_A, *local_x, *local_y;
  int succ, all_succ, blocksize, i, j, local_i;
  char *msg;

  blocksize = n / no_proc; /* n is divisible by no_proc by assunmption */
  succ = allocate_space(&local_A, &local_x, &local_y, blocksize, n);
  if (succ == 0) { /* one of processes failed in memory
                     allocation */
    msg = "Failed space allocation";
    if (my_rank == 0) printf("%s %s\n", testmsg, msg);
    return msg;
  }

  /* Initialize test matrix and vectors */
  for (i = 0; i < n; i++) {
    if (procmap(i, blocksize) == my_rank) {
      local_i = local(i, blocksize);
      local_x[local_i] = i;
    }
  }
  for (i = 0; i < blocksize; i++) {
    for (j = 0; j < n; j++) local_A[i * n + j] = c;
  }

  if (my_rank == 0) startwtime = MPI_Wtime();
  succ = mat_vect_mult(local_A, local_x, local_y, blocksize, n, my_rank,
                       no_proc, comm);
  if (succ == 0) { /* one of processes failed in computing */
    msg = "Failed matrix multiplication";
    if (my_rank == 0) printf("%s %s\n", testmsg, msg);
    return msg;
  }

  if (my_rank == 0) {
    endwtime = MPI_Wtime();
    printf("%s: Wall clock time = %f at Proc 0 of %d processes\n", testmsg,
           endwtime - startwtime, no_proc);
  }

  msg = validate_vect(local_y, blocksize, n, c);
  free(local_A);
  free(local_x);
  free(local_y);
  succ = (msg == NULL);
  /* Proc 0 collects the correctness status from all processes */
  MPI_Reduce(&succ, &all_succ, 1, MPI_INT, MPI_PROD, 0, comm);
  if (all_succ) return NULL;
  /* one of processes failed in validation */
  msg = "Failed to validate multiplication results";
  if (my_rank == 0) printf("%s %s\n", testmsg, msg);
  return msg;
}

char *mv_test1() { return mv_test("Test 1", 100, 1); }

char *mv_test2() { return mv_test("Test 2", 1024, 2); }

/*-------------------------------------------------------------------
 * Run all tests.  Ignore returned messages.
 */
void run_all_tests(void) {
  mu_run_test(mv_test1);
  mu_run_test(mv_test2);
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
