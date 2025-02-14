/* File:     itmv_mult_omp.c
 *
 * Purpose:  Implement iteartive parallel matrix-vector multiplication with
 * OpenMP. Use one-dimensional arrays to store the vectors and the matrix.
 *
 * Algorithm:
 *       For k=0 to t-1
 *            y = d+ Ax
 *     x=y
 *          Endfor
 */
#include "itmv_mult_omp.h"
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------
 * Function:            mv_compute
 * Purpose:             Compute  y[i]=d[i]+A[i]x;
 * In arg:              i -- row index
 * Global in vars:
 *        matrix_A:  2D matrix A represented by a 1D array.
 *        vector_d:  vector d
 *        matrix_type:  matrix_type=0 means A is a regular matrix.
 *            matrix_type=1 (UPPER_TRIANGULAR) means A is an upper
 * triangular matrix
 *        matrix_dim:  the global  number of columns (same as the
 * number of rows)
 *
 * Global in/out vars:
 *            vector_x:  vector x
 * Global out vars:
 *            vector_y:  vector y
 */
void mv_compute(int i) {
  int j, col_start, k;
  vector_y[i] = vector_d[i];
  if (matrix_type == UPPER_TRIANGULAR) {
    col_start = i;
  } else {
    col_start = 0;
  }
  for (j = col_start; j < matrix_dim; j++) {
    vector_y[i] += matrix_A[i * matrix_dim + j] * vector_x[j];
  }
}
/*---------------------------------------------------------------------
 * Function:            parallel_itmv_mult
 * Purpose:             Run t iterations of parallel computation in parallel:
 * {y=d+Ax; x=y}
 *
 * In arg:              threadcnt - number of threads to run in parallel
 *                      mappingtype:  BLOCK_MAPPING, BLOCK_CYLIC, BLOCK_DYNAMIC. 
 *                      These constants are defined in itmv_mult_omp.h 
 *                They correspond to OpenMP's omp_sched_static with block size equal 
 *                to number of iterations  divided by number of threads, 
 *              omp_sched_static with basic chunk size equal to  chunksize, 
 *              omp_sched_dynamic with basic chunk size equal to chunksize. 
 *            Type  omp_sched_guided is not required.
 *
 * Global in vars: matrix_A:  2D matrix A represented by a 1D array.
 *                 vector_d:  vector d
 *                 matrix_type:  matrix_type=0 means A is a regular matrix.
 *                    matrix_type=1 (UPPER_TRIANGULAR) means A is
 *                    an upper triangular matrix
 *                 matrix_dim:  the global  number of columns
 *                      (same as the number of rows)
 *                 no_iterations:  the number of iterations
 *
 * Global in/out vars:
 *                 vector_x:  vector x
 * Global out vars:
 *                 vector_y:  vector y
 */
void parallel_itmv_mult(int threadcnt, int mappingtype, int chunksize) {
  /*Your solutuion with OpenMP*/
  int i, k;

  for (k = 0; k < no_iterations; k++) {
    omp_set_num_threads(threadcnt);
    omp_sched_t type;
    if(mappingtype == BLOCK_MAPPING)
      type = omp_sched_static;
    else if(mappingtype == BLOCK_CYCLIC)
      type = omp_sched_static;
    else if(mappingtype == BLOCK_DYNAMIC)
      type = omp_sched_dynamic;
    else if(mappingtype == BLOCK_GUIDED)
      type = omp_sched_guided;

    omp_set_schedule(type, chunksize);
    #pragma omp parallel for private(i)
    for (i = 0; i < matrix_dim; i++) {
      mv_compute(i);
    }

    #pragma omp parallel for private(i)
    for (i = 0; i < matrix_dim; i++) {
      vector_x[i] = vector_y[i];
    }
  }
}

/*-------------------------------------------------------------------
 * Function:  itmv_mult_seq
 * Purpose:   Run t iterations of  computation:  {y=d+Ax; x=y} sequentially.
 * In args:   A:  matrix A
 *            d:  column vector d
 *            matrix_type:  matrix_type=0 means A is a regular matrix.
 *                  matrix_type=1 (UPPER_TRIANGULAR) means A is an upper
 *    triangular matrix
 *            n:        the global  number of columns (same as the number
 *    of rows)
 *            t:  the number of iterations
 *
 * In/out:    x:  column vector x
 *            y:  column vector y
 *
 * Return:  1  means succesful 0  means unsuccessful
 *
 * Errors: If an error is detected (e.g. n is non-positive),
 *    matrix/vector pointers are NULL.
 *
 */
int itmv_mult_seq(double A[], double x[], double d[], double y[],
                  int matrix_type, int n, int t) {
  int i, j, start, k;

  if (n <= 0 || A == NULL || x == NULL || d == NULL || y == NULL) return 0;

  for (k = 0; k < t; k++) {
    for (i = 0; i < n; i++) {
      y[i] = d[i];
      if (matrix_type == UPPER_TRIANGULAR) {
        start = i;
      } else {
        start = 0;
      }
      for (j = start; j < n; j++) {
        y[i] += A[i * n + j] * x[j];
      }
    }
    for (i = 0; i < n; i++) {
      x[i] = y[i];
    }
  }
  return 1;
}
