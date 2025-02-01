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
 *  A[i][j]=c in all positions.
 *  y[i] is 0 in all positions x[i]= i for 0<=i<n For simplicity,
 *  we assume n is divisible by no_proc.
 */

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

int my_rank, no_proc;
MPI_Comm comm;

#define MAX_TEST_MATRIX_SIZE 2048

#define FAIL 0
#define SUCC 1

/*Block mapping functions*/
#define procmap(i, r) ((int)floor((double)i / r))
#define local(i, r) (i % r)

#define TEST_CORRECTNESS 1
#define UPPER_TRIANGULAR 1
int itmv_mult(double local_A[] /* in */, double local_x[] /* in */,
              double local_d[] /* in */, double local_y[] /* out */,
              double global_x[] /* out */, int matrix_type /* in */,
              int n /* in */, int t /* in */, int blocksize /* in */,
              int my_rank /* in */, int no_proc /* in */,
              MPI_Comm comm /* in */);

int itmv_mult_seq(double A[], double x[], double d[], double y[],
                  int matrix_type, int n, int t);

void print_error(char *msgheader, char *msg) {
  if (my_rank == 0) {
    printf("%s Proc 0 error: %s\n", msgheader, msg);
  }
}

void print_itmv_sample(char *msgheader, double A[], double x[], double d[],
                       double y[], int matrix_type, int n, int t) {
  printf("%s Test matrix type %d, size n=%d, t=%d\n", msgheader, matrix_type, n,
         t);
  if (n < 4 || A == NULL || x == NULL || d == NULL || y == NULL) return;
  printf("%s check x[0-3] %f, %f, %f, %f\n", msgheader, x[0], x[1], x[2], x[3]);
  printf("%s check d[0-3] are %f, %f, %f, %f\n", msgheader, d[0], d[1], d[2],
         d[3]);
  printf("%s check A[0][0-3] are %f, %f, %f, %f\n", msgheader, A[0], A[1], A[2],
         A[3]);
  printf("%s check A[1][0-3] are %f, %f, %f, %f\n", msgheader, A[n], A[n + 1],
         A[n + 2], A[n + 3]);
  printf("%s check A[2][0-3] are %f, %f, %f, %f\n", msgheader, A[2 * n],
         A[2 * n + 1], A[2 * n + 2], A[2 * n + 3]);
  printf("%s check A[3][0-3] are %f, %f, %f, %f\n", msgheader, A[3 * n],
         A[3 * n + 1], A[3 * n + 2], A[3 * n + 3]);
}

void print_itmv_sample_distributed(char *msgheader, double local_A[],
                                   double local_x[], double local_d[],
                                   double local_y[], int matrix_type, int n,
                                   int t, int blocksize) {
  int i, local_i;

  printf("%s Distributed blocksize=%d matrix type %d, size n=%d, t=%d\n",
         msgheader, blocksize, matrix_type, n, t);
  if (n < 4) return;
  for (i = 0; i < n; i++) {
    if (procmap(i, blocksize) == my_rank) {
      local_i = local(i, blocksize);
      printf("%s Proc %d x[%d] locally x[%d] = %f; d[%d] locally d[%d] = %f\n",
             msgheader, my_rank, i, local_i, local_x[local_i], i, local_i,
             local_d[local_i]);
      printf(
          "%s Proc %d Row A[%d] locally A[%d] last 4 elements = %f, %f, %f, "
          "%f\n",
          msgheader, my_rank, i, local_i, local_A[local_i * n + n - 4],
          local_A[local_i * n + n - 3], local_A[local_i * n + n - 2],
          local_A[local_i * n + n - 1]);
    }
  }
}

/*----------------------------------------------------------------------------
 * Test if the t iterations of parallel computation {y=Ax;  x=y} matches the
 * expectation. If failed, return a message string showing the failed point If
 * successful, return NULL
 *
 * In args:
 *  global_x[] is the  array hosted at process 0 storing the final result;
 *  n is the number of columns (and rows);
 *  t is the number of iterations conducted;
 *  matrix_type being 0 means regular matrix;
 *  matrix_type being 1 (UPPER_TRIANGULAR) means upper triangular.
 *
 * Return:
 *  a column vector that contains the final result column vector y.
 *
 * Note:
 *  We test only for small n value, and thus we will simplly run
 *  sequential code to obtain the expected vector and then compare.
 */
double *compute_expected(char *testmsg, int n, int t, int matrix_type) {
  int i, j, start;
  double *A, *x, *d, *y;

  A = malloc(n * n * sizeof(double));
  x = malloc(n * sizeof(double));
  d = malloc(n * sizeof(double));
  y = malloc(n * sizeof(double));
  /* Here we assume none of them are NULL. given a modest size n */
  for (i = 0; i < n; i++) {
    x[i] = 0;
    d[i] = (2.0 * n - 1.0) / n;
  }
  for (i = 0; i < n; i++) {
    A[i * n + i] = 0.0;
    if (matrix_type == UPPER_TRIANGULAR)
      start = i + 1;
    else
      start = 0;
    for (j = start; j < n; j++) {
      if (i != j) A[i * n + j] = -1.0 / n;
    }
  }
#ifdef DEBUG1
  print_itmv_sample(testmsg, A, x, d, y, matrix_type, n, t);
#endif
  itmv_mult_seq(A, x, d, y, matrix_type, n, t);

  free(A);
  free(x);
  free(d);
  return y;
}

char *validate_vect(char *msgheader, double global_x[], int n, int t,
                    int matrix_type) {
  int i;
  double *expected;

  if (n <= 0) return "Failed: 0 or negative size";
  if (n > MAX_TEST_MATRIX_SIZE) return "Failed: Too big to validate";

  expected = compute_expected(msgheader, n, t, matrix_type);
  for (i = 0; i < n; i++) {
#ifdef DEBUG1
    printf("%s Proc 0: i=%d  Expected %f Actual %f\n", msgheader, i,
           expected[i], global_x[i]);
#endif
    mu_assert("One mismatch in iterative mat-vect multiplication",
              global_x[i] == expected[i]);
  }
  free(expected);
  return NULL;
}

/*-------------------------------------------------------------------
 * Allocate storage space for each array at each processs.
 * If failed, 0
 * If successful, return 1
 *
 * In args:
 *  *local_A is the starting address of local space for matrix A
 *  with rowwise block mapping;
 *  *local_x is the starting address of local space for vector x
 *  with rowwise block mapping;
 *  *local_d is the starting address of local space for vector d
 *  with rowwise block mapping;
 *  *local_y is the starting address of local space for vector y;
 *  with rowwise block mapping;
 *  *global_x is the starting address of space for vector x
 *  with size n blocksize is the size of local array;
 *  n is the number of columns (and rows).
 */

int allocate_space(double **local_A, double **local_x, double **local_d,
                   double **local_y, double **global_x, int blocksize, int n) {
  int succ = 1, all_succ = 1;

  *local_A = malloc(blocksize * n * sizeof(double));
  *local_x = malloc(blocksize * sizeof(double));
  *local_d = malloc(blocksize * sizeof(double));
  *local_y = malloc(blocksize * sizeof(double));
  *global_x = malloc(n * sizeof(double));
  /* Here we assume none of them are NULL. A more robust program needs to
     check the failed memory allocation */
  if (*local_A == NULL || *local_x == NULL || *local_d == NULL ||
      *local_y == NULL || *global_x == NULL) {
    /* Find an error, thus we release space first */
    if (*local_A != NULL) free(*local_A);
    if (*local_x != NULL) free(*local_x);
    if (*local_d != NULL) free(*local_d);
    if (*local_y != NULL) free(*local_y);
    if (*global_x != NULL) free(*global_x);
    succ = 0;
  }
  /* All processes synchronize to check if there is any failure in allocation
   */
  MPI_Allreduce(&succ, &all_succ, 1, MPI_INT, MPI_PROD, comm);
  return all_succ;
}

/*-------------------------------------------------------------------
 * Initialize test matrix and vectors distributed at each processs.
 *    vector x of size n: 0 for every element
 *    vector d of size n: (2n-1)/n for every element
 *
 * When matrix_type is not UPPER_TRIANGULAR, matrix A of size nxn:
 *    A[i,i]=0 for diagnal elements. A[i,j]=-1/n for non-diagonal elements.
 * When matrix_type is UPPER_TRIANGULAR matrix  A of size nxn:
 *    A[i,i]=0 for diagnal elements. A[i,j]=-1/n for upper diagonal elements
 *    with i<j all lower triangular elements are 0
 *
 * In args:
 *  *local_A is the starting address of local space for matrix A
 *  with rowwise block mapping, # of rows=blocksize;
 *  *local_x is the starting address of local space for vector x
 *  with rowwise block mapping, # of elements=blocksize;
 *  *local_d is the starting address of local space for vector d
 *  with rowwise block mapping, # of elements=blocksize;
 *  *local_y is the starting address of local space for vector y
 *  with rowwise block mapping, # of elements=blocksize;
 *  blocksize is the size of local array;
 *  n is the number of columns (and rows);
 *  marix_type: matrix type UPP_TRIANGULAR means A is upper triangular
 *  otherwise just reguar square matrix;
 *  my_rank: my process ID (started from 0);
 *
 * Return value:
 *  If failed, return FAIL (0); If successful, return 1 (SUCC)
 *
 * Global variable: This function should NOT use any global variable
 */
int init_matrix(double *local_A, double *local_x, double *local_d,
                double *local_y, int blocksize, int n, int matrix_type,
                int my_rank) {
  if (local_A == NULL || local_x == NULL || local_d == NULL ||
      local_y == NULL || blocksize <= 0)
    return FAIL;

  /* Your solution */
  for(int i=0; i<blocksize; i++) local_x[i] = 0.0;
  for(int i=0; i<blocksize; i++) local_d[i] = (2.0*n-1.0) / n;
  int global_row;
  for(int i=0; i<blocksize; i++) {
    global_row = my_rank * blocksize + i;
    for(int j=0; j<n; j++) {
      if(matrix_type != UPPER_TRIANGULAR) {
        if(global_row == j) {
          local_A[i*n+j] = 0.0;
        } else {
          local_A[i*n+j] = -1.0/n;
        }
      } else {
        if(global_row < j) {
          local_A[i*n+j] = -1.0/n;
        } else {
          local_A[i*n+j] = 0.0;
        }
      }
    }
  }
  return SUCC;
}

/*-------------------------------------------------------------------
 * Test matrix vector multiplication
 * Process 0 collects the  error detection. If failed, return a message string
 * If successful, return NULL
 */
char *itmv_test(char *testmsg, int test_correctness, int n, int matrix_type,
                int t) {
  double startwtime = 0, endwtime = 0;
  double *local_A, *local_x, *local_d, *local_y, *global_x;
  int succ, all_succ, blocksize, i, j, local_i, start;
  char *msg;

  blocksize = n / no_proc; /* n is divisible by no_proc by assunmption */
  succ = allocate_space(&local_A, &local_x, &local_d, &local_y, &global_x,
                        blocksize, n);
  if (succ == 0) { /* one of processes failed in memory
                      allocation */
    msg = "Failed space allocation";

    print_error(testmsg, msg);
    return msg;
  }
  succ = init_matrix(local_A, local_x, local_d, local_y, blocksize, n,
                     matrix_type, my_rank);

#ifdef DEBUG1
  print_itmv_sample_distributed(testmsg, local_A, local_x, local_d, local_y,
                                matrix_type, n, t, blocksize);
#endif

  if (my_rank == 0) startwtime = MPI_Wtime();

  succ = itmv_mult(local_A, local_x, local_d, local_y, global_x, matrix_type, n,
                   t, blocksize, my_rank, no_proc, comm);
  if (succ == 0) { /* one of processes failed in computing */
    msg = "Failed matrix multiplication";
    print_error(testmsg, msg);
    return msg;
  }

  if (my_rank == 0) {
    endwtime = MPI_Wtime();
    printf("%s: Wall clock time = %f at Proc 0 of %d processes\n", testmsg,
           endwtime - startwtime, no_proc);
  }
  msg = NULL;
  if (test_correctness == TEST_CORRECTNESS) {
    if (my_rank == 0) {
      msg = validate_vect(testmsg, global_x, n, t, matrix_type);
      if (msg != NULL) print_error(testmsg, msg);
    }
  }
  free(local_A);
  free(local_x);
  free(local_y);
  free(local_d);
  free(global_x);
  return msg; /* Only process 0 conducts correctness test,
                 and prints summary report */
}

char *itmv_seq_test1() {
    if (my_rank != 0) return NULL;  // Only run on processor 0
    
    int n = 4096;  // Size for testing
    int t = 1024;  // Number of iterations
    double *A, *x, *d, *y;
    char *msg = NULL;
    double startwtime, endwtime;

    // Allocate memory
    A = malloc(n * n * sizeof(double));
    x = malloc(n * sizeof(double));
    d = malloc(n * sizeof(double));
    y = malloc(n * sizeof(double));

    if (A == NULL || x == NULL || d == NULL || y == NULL) {
        return "Failed space allocation";
    }

    // Initialize data
    for (int i = 0; i < n; i++) {
        x[i] = 0.0;
        d[i] = (2.0*n-1.0) / n;
        
        for (int j = 0; j < n; j++) {
            if (i == j) {
                A[i*n + j] = 0.0;
            } else {
                A[i*n + j] = -1.0/n;
            }
        }
    }

    startwtime = MPI_Wtime();
    int success = itmv_mult_seq(A, x, d, y, 0, n, t);
    endwtime = MPI_Wtime();
    printf("Seq Test 1 (Regular Matrix): Wall clock time = %f seconds\n", 
           endwtime - startwtime);

    if (!success) {
        msg = "Sequential multiplication failed";
    }

    // Cleanup
    free(A);
    free(x);
    free(d);
    free(y);

    return msg;
}

char *itmv_seq_test2() {
    if (my_rank != 0) return NULL;  // Only run on processor 0
    
    int n = 4096;
    int t = 1024;
    double *A, *x, *d, *y;
    char *msg = NULL;
    double startwtime, endwtime;

    // Allocate memory
    A = malloc(n * n * sizeof(double));
    x = malloc(n * sizeof(double));
    d = malloc(n * sizeof(double));
    y = malloc(n * sizeof(double));

    if (A == NULL || x == NULL || d == NULL || y == NULL) {
        return "Failed space allocation";
    }

    // Initialize data for upper triangular matrix
    for (int i = 0; i < n; i++) {
        x[i] = 0.0;
        d[i] = (2.0*n-1.0) / n;

        for (int j = 0; j < n; j++) {
            if (i <= j) {
                if (i == j) {
                    A[i*n+j] = 0.0;
                } else {
                    A[i*n+j] = -1.0/n;
                }
            } else {
                A[i*n+j] = 0.0;
            }
        }
    }

    // Start timing
    startwtime = MPI_Wtime();
    
    // Run sequential multiplication
    int success = itmv_mult_seq(A, x, d, y, UPPER_TRIANGULAR, n, t);
    
    // End timing
    endwtime = MPI_Wtime();
    printf("Sequential Test 2 (Upper Triangular): Wall clock time = %f seconds\n", 
           endwtime - startwtime);

    if (!success) {
        msg = "Sequential multiplication failed for upper triangular";
    }

    // Cleanup
    free(A);
    free(x);
    free(d);
    free(y);

    return msg;
}

char *itmv_test1() {
  return itmv_test("Test 1", TEST_CORRECTNESS, 4, !UPPER_TRIANGULAR, 1);
}

char *itmv_test2() {
  return itmv_test("Test 2", TEST_CORRECTNESS, 4, !UPPER_TRIANGULAR, 2);
}

char *itmv_test3() {
  return itmv_test("Test 3", TEST_CORRECTNESS, 8, !UPPER_TRIANGULAR, 1);
}

char *itmv_test4() {
  return itmv_test("Test 4", TEST_CORRECTNESS, 8, !UPPER_TRIANGULAR, 2);
}

char *itmv_test5() {
  return itmv_test("Test 5", TEST_CORRECTNESS, 4, UPPER_TRIANGULAR, 1);
}

char *itmv_test6() {
  return itmv_test("Test 6", TEST_CORRECTNESS, 4, UPPER_TRIANGULAR, 2);
}

char *itmv_test7() {
  return itmv_test("Test 7", TEST_CORRECTNESS, 8, UPPER_TRIANGULAR, 1);
}

char *itmv_test8() {
  return itmv_test("Test 8", TEST_CORRECTNESS, 8, UPPER_TRIANGULAR, 2);
}

char *itmv_test9() {
  return itmv_test("Test 9: n=2K t=1K", !TEST_CORRECTNESS, 2048,
                   !UPPER_TRIANGULAR, 1024);
}

char *itmv_test10() {
  return itmv_test("Test 10: n=2K t=1K upper", !TEST_CORRECTNESS, 2048,
                   UPPER_TRIANGULAR, 1024);
}

char *itmv_test11() {
  return itmv_test("Test 11: n=4K t=1K", !TEST_CORRECTNESS, 4096,
                   !UPPER_TRIANGULAR, 1024);
}

char *itmv_test12() {
  return itmv_test("Test 12: n=4K t=1K upper", !TEST_CORRECTNESS, 4096,
                   UPPER_TRIANGULAR, 1024);
}

char *itmv_test13() {
  return itmv_test("Test 13: n=8K t=1K", !TEST_CORRECTNESS, 4096 * 2,
                   !UPPER_TRIANGULAR, 1024);
}

char *itmv_test14() {
  return itmv_test("Test 14: n=8K t=1K upper", !TEST_CORRECTNESS, 4096 * 2,
                   UPPER_TRIANGULAR, 1024);
}

/*-------------------------------------------------------------------
 * Run all tests.  Ignore returned messages.
 */
void run_all_tests(void) {
  mu_run_test(itmv_test1);
  mu_run_test(itmv_test2);
  mu_run_test(itmv_test3);
  mu_run_test(itmv_test4);
  mu_run_test(itmv_test5);
  mu_run_test(itmv_test6);
  mu_run_test(itmv_test7);
  mu_run_test(itmv_test8);
  /* mu_run_test(itmv_test9); mu_run_test(itmv_test10); */
  mu_run_test(itmv_test11); mu_run_test(itmv_test12);
  mu_run_test(itmv_seq_test1);
  mu_run_test(itmv_seq_test2);
  /* mu_run_test(itmv_test13); mu_run_test(itmv_test14); */
}

/*-------------------------------------------------------------------
 * The main entrance to run all tests.
 * Only Proc 0 prints the test summary
 */
void testmain() {
  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &no_proc);
  MPI_Comm_rank(comm, &my_rank);

  run_all_tests();

  if (my_rank == 0) {
    mu_print_test_summary("Summary:");
  }
}
