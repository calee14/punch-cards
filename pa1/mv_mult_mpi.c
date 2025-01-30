/* File:
 *  mv_mult_mpi.c
 *
 * Purpose:
 *  Implement parallel matrix-vector multiplication using
 *  one-dimensional arrays to store the vectors and the
 *  matrix.  Vectors use block distributions and the
 *  matrix is distributed by block rows.
 *
 * Input:
 *  Rowwise-distributed matrix and vectors A, y, x,
 *
 * Output:
 *  Product vector y = Ax.
 *
 * Notes:
 *    1. Number of processes should evenly divide both m and n
 *    2. Define DEBUG for verbose output
 *
 * IPP:      Section 3.4.9 (pp. 113 and ff.)
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/*-------------------------------------------------------------------
 * Function:
 *  mat_vect_mult
 *
 * Purpose:
 *  Multiply a matrix A by a vector x.  The matrix is distributed
 *  by block rows and the vectors are distributed by blocks.
 *
 * In args:
 *  local_A: calling process' rows of matrix A
 *  local_x: calling process' components of vector x
 *  blocksize: The size of local array local_x  and it is n/no_proc.
 *  n: the global  number of columns (same as the number of rows)
 *  my_rank: local process ID
 *  no_proc: no of processes
 *  comm: communicator containing all calling processes
 *
 * Return:
 *  1 means succesful 0 means unsuccessful
 *
 * Errors:
 *  If an error is detected (e.g. n is non-positive.
 *  n is not evenly divisible by the number of processes, malloc fails),
 *  the program returns 0.
 */
int mat_vect_mult(double local_A[] /* in */, double local_x[] /* in */,
                  double local_y[] /* out */, int blocksize /* in */,
                  int n /* in */, int my_rank /* in */, int no_proc /* in */,
                  MPI_Comm comm /* in */) {
  double *x;
  int local_i, j;
  int succ, all_succ;

  if (n <= 0 || blocksize <= 0 || local_A == NULL || local_x == NULL ||
      local_y == NULL || no_proc <= 0 || my_rank < 0 || my_rank >= no_proc)
    return 0;
  if (n % no_proc != 0) /* n is not divisible by no_proc */
    return 0;
  if (blocksize != (n / no_proc)) /* has to n/no_proc */
    return 0;
  x = malloc(n * sizeof(double));
  succ = x != NULL;
  MPI_Allreduce(&succ, &all_succ, 1, MPI_INT, MPI_PROD, comm);
  if (all_succ == 0) return 0;

  MPI_Allgather(local_x, blocksize, MPI_DOUBLE, x, blocksize, MPI_DOUBLE, comm);

  for (local_i = 0; local_i < blocksize; local_i++) {
    local_y[local_i] = 0;
    int init_j = 0;
    if(matrix_type == UPPER_TRIANGULAR) init_j = local_i;
    for (j = init_j; j < n; j++) local_y[local_i] += local_A[local_i * n + j] * x[j];
  }
  free(x);
  return 1;
} /* mat_vect_mult */
