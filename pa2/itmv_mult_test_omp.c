/*
 * File:     itmv_mult_test_omp.c
 *
 * Purpose:  test matrix vector multiplication y=Ax.
 * Matrix A is a square matrix of size nxn. Column vectors x and y are of size
 * nx1 Input test matrix:  A[i][j]=c in all positions.  y[i] is 0 in all
 * positions x[i]= i for 0<=i<n For simplicity, we assume n is divisible by
 * no_proc
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "itmv_mult_omp.h"
#include "minunit.h"

#define MAX_TEST_MATRIX_SIZE 256

#define TEST_CORRECTNESS 1

/*Global variables*/
double *matrix_A;
double *vector_x;
double *vector_d;
double *vector_y;
int matrix_type;
int matrix_dim;
int no_iterations;
int thread_count;
int thread_mapping = BLOCK_MAPPING;
int cyclic_blocksize;

int itmv_mult_seq(double A[], double x[], double d[], double y[],
                  int matrix_type, int n, int t);

void print_error(char *msgheader, char *msg) {
  printf("%s error msg: %s\n", msgheader, msg);
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

/*----------------------
 * Initialize the test data
 */

void initialize(double A[], double x[], double d[], double y[], int n,
                int matrix_type) {
  /*Here we assume none of them are NULL. given a modest size n*/
  int i, j, start;
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
}

/*-------------------------------------------------------------------------------
 * Test if the  t iterations of parallel computation {y=Ax;  x=y} matches the
 * expectation. If failed, return a message string showing the failed point If
 * successful, return NULL
 *
 * In args: n is the number of columns (and rows) t is
 *    the number of iterations conducted.
 *          matrix_type: 0 means regular matrix. 1
 *    (UPPER_TRIANGULAR) means upper triangular.
 *
 * Return:  a column vector that contains the final result column vector y.
 *    Note: We test only for small n value, and thus we will simplly run
 *    sequential code to obtain the expected vector and then compare.
 */
double *compute_expected(char *testmsg, int n, int t, int matrix_type) {
  double *A, *x, *d, *y;
  A = malloc(n * n * sizeof(double));
  x = malloc(n * sizeof(double));
  d = malloc(n * sizeof(double));
  y = malloc(n * sizeof(double));

  initialize(A, x, d, y, n, matrix_type);
#ifdef DEBUG1
  print_itmv_sample(testmsg, A, x, d, y, matrix_type, n, t);
#endif
  itmv_mult_seq(A, x, d, y, matrix_type, n, t);

  free(A);
  free(x);
  free(d);
  return y;
}

char *validate_vect(char *msgheader, double y[], int n, int t,
                    int matrix_type) {
  int i;
  double *expected;
  if (n <= 0) return "Failed: 0 or negative size";
  if (n > MAX_TEST_MATRIX_SIZE) return "Failed: Too big to validate";

  expected = compute_expected(msgheader, n, t, matrix_type);
  for (i = 0; i < n; i++) {
#ifdef DEBUG1
    printf("%s y[i=%d]  Expected %f Actual %f\n", msgheader, i, expected[i],
           y[i]);
#endif
    mu_assert("One mismatch in iterative mat-vect multiplication",
              y[i] == expected[i]);
  }
  free(expected);
  return NULL;
}

/*-------------------------------------------------------------------
 * Allocate storage space for each array at each processs.
 * If failed, 0
 * If successful, return 1
 * In args:
 *    *matrix_A is the starting address of space for matrix A
 *    *vector_x is the starting address of space for vector x
 *    *vector_d is the starting address of space for vector d
 *    *vector_y is the starting address of space for vector y
 *  n is the number of columns (and rows)
 */
int allocate_space(double **A, double **x, double **d, double **y, int n) {
  int succ = 1;
  *A = malloc(n * n * sizeof(double));
  *x = malloc(n * sizeof(double));
  *d = malloc(n * sizeof(double));
  *y = malloc(n * sizeof(double));
  if (*A == NULL || *x == NULL || *d == NULL || *y == NULL) {
    /*Find an error, thus we release space first*/
    if (*A != NULL) free(*A);
    if (*x != NULL) free(*x);
    if (*d != NULL) free(*d);
    if (*y != NULL) free(*y);
    succ = 0;
  }
  return succ;
}

/*-------------------------------------------------------------------
 * Test matrix vector multiplication
 * Process 0 collects the  error detection. If failed, return a message string
 * If successful, return NULL
 */
char *itmv_test(char *testmsg, int test_correctness, int n, int mtype, int t,
                int mappingtype, int cyclic_block) {
  double startwtime = 0, endwtime = 0;
  int succ;
  char *msg;

  matrix_dim = n;
  no_iterations = t;
  matrix_type = mtype;
  thread_mapping = mappingtype;
  cyclic_blocksize = cyclic_block;

  succ = allocate_space(&matrix_A, &vector_x, &vector_d, &vector_y, n);
  if (succ == 0) { /*one of processes failed in memory allocation*/
    msg = "Failed space allocation";
    print_error(testmsg, msg);
    return msg;
  }
  /*Initialize test matrix and vectors*/
  initialize(matrix_A, vector_x, vector_d, vector_y, n, matrix_type);
#ifdef DEBUG1
  print_itmv_sample(testmsg, matrix_A, vector_x, vector_d, vector_y,
                    matrix_type, n, t);
#endif
  startwtime = get_time();

  parallel_itmv_mult(thread_count, mappingtype, cyclic_block);

  endwtime = get_time();
  printf("%s: Wall clock time = %f with %d threads\n", testmsg,
         endwtime - startwtime, thread_count);

  msg = NULL;
  if (test_correctness == TEST_CORRECTNESS) {
    msg = validate_vect(testmsg, vector_y, n, t, matrix_type);
    if (msg != NULL) {
      print_error(testmsg, msg);
    }
  }
  free(matrix_A);
  free(vector_x);
  free(vector_y);
  free(vector_d);
  return msg; /*Only process 0 conducts correctness test, and prints summary
                 report*/
}

char *itmv_test1() {
  return itmv_test("Test 1", TEST_CORRECTNESS, 16, !UPPER_TRIANGULAR, 2,
                   BLOCK_MAPPING, 0);
}
char *itmv_test2() {
  return itmv_test("Test 2", TEST_CORRECTNESS, 16, !UPPER_TRIANGULAR, 2,
                   BLOCK_CYCLIC, 2);
}
char *itmv_test3() {
  return itmv_test("Test 3", TEST_CORRECTNESS, 16, UPPER_TRIANGULAR, 2,
                   BLOCK_MAPPING, 0);
}
char *itmv_test4() {
  return itmv_test("Test 4", TEST_CORRECTNESS, 16, UPPER_TRIANGULAR, 2,
                   BLOCK_CYCLIC, 2);
}
char *itmv_test5() {
  return itmv_test("Test 5", TEST_CORRECTNESS, 17, !UPPER_TRIANGULAR, 2,
                   BLOCK_MAPPING, 2);
}
char *itmv_test6() {
  return itmv_test("Test 6", TEST_CORRECTNESS, 17, !UPPER_TRIANGULAR, 2,
                   BLOCK_CYCLIC, 2);
}
char *itmv_test6a() {
  return itmv_test("Test 6a n=16 dynamic 2", TEST_CORRECTNESS, 17,
                   UPPER_TRIANGULAR, 2, BLOCK_DYNAMIC, 2);
}
char *itmv_test7() {
  return itmv_test("Test 7", TEST_CORRECTNESS, 17, UPPER_TRIANGULAR, 2,
                   BLOCK_MAPPING, 0);
}
char *itmv_test8() {
  return itmv_test("Test 8", TEST_CORRECTNESS, 17, UPPER_TRIANGULAR, 2,
                   BLOCK_CYCLIC, 2);
}
char *itmv_test8a() {
  return itmv_test("Test 8a n=17 dynamic 2", TEST_CORRECTNESS, 17,
                   UPPER_TRIANGULAR, 2, BLOCK_DYNAMIC, 2);
}

char *itmv_test9() {
  return itmv_test("Test 9: n=4K t=1K blockmapping", !TEST_CORRECTNESS, 4096,
                   !UPPER_TRIANGULAR, 1024, BLOCK_MAPPING, 0);
}
char *itmv_test10() {
  return itmv_test("Test 10: n=4K t=1K block cylic (r=1)", !TEST_CORRECTNESS,
                   4096, !UPPER_TRIANGULAR, 1024, BLOCK_CYCLIC, 1);
}
char *itmv_test11() {
  return itmv_test("Test 11: n=4K t=1K block cyclic (r=16)", !TEST_CORRECTNESS,
                   4096, !UPPER_TRIANGULAR, 1024, BLOCK_CYCLIC, 16);
}
char *itmv_test11a() {
  return itmv_test("Test 11a: n=4K t=1K dynamic (r=16)", !TEST_CORRECTNESS,
                   4096, !UPPER_TRIANGULAR, 1024, BLOCK_DYNAMIC, 16);
}
char *itmv_test12() {
  return itmv_test("Test 12: n=4K t=1K upper block mapping", !TEST_CORRECTNESS,
                   4096, UPPER_TRIANGULAR, 1024, BLOCK_MAPPING, 0);
}
char *itmv_test13() {
  return itmv_test("Test 13: n=4K t=1K upper block cylic (r=1)",
                   !TEST_CORRECTNESS, 4096, UPPER_TRIANGULAR, 1024,
                   BLOCK_CYCLIC, 1);
}
char *itmv_test14() {
  return itmv_test("Test 14: n=4K t=1K upper block cyclic(r=16)",
                   !TEST_CORRECTNESS, 4096, UPPER_TRIANGULAR, 1024,
                   BLOCK_CYCLIC, 16);
}
char *itmv_test14a() {
  return itmv_test("Test 14a: n=4K t=1K upper dynamic(r=16)", !TEST_CORRECTNESS,
                   4096, UPPER_TRIANGULAR, 1024, BLOCK_DYNAMIC, 16);
}

/*-------------------------------------------------------------------
 * Run all tests.  Ignore returned messages.
 * Only measure and report parallel performance for the upper triangular test
 * matrix.
 */
void run_all_tests(void) {
  mu_run_test(itmv_test1);
  mu_run_test(itmv_test2);
  mu_run_test(itmv_test3);
  mu_run_test(itmv_test4);
  mu_run_test(itmv_test5);
  mu_run_test(itmv_test6);
  mu_run_test(itmv_test6a);
  mu_run_test(itmv_test7);
  mu_run_test(itmv_test8);
  mu_run_test(itmv_test8a);

  /*
  mu_run_test(itmv_test12);
  mu_run_test(itmv_test13);
  mu_run_test(itmv_test14);
  mu_run_test(itmv_test14a);
  */
}

/*-------------------------------------------------------------------
 * The main entrance to run all tests.
 */
int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("incorrect # of arguments");
    printf("./itmv_mult_test_omp <number of threads> \n");
    return 1;
  }
  thread_count = atoi(argv[1]);
  if (thread_count <= 0 || thread_count > THREAD_COUNT_MAX) {
    printf("The number of threads is not positive or too big\n");
    return 1;
  }
  run_all_tests();
  mu_print_test_summary("Summary:");
  return 0;
}
