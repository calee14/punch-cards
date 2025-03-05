/*
 * File:     it_mult_vec_test.cu
 *
 * Purpose:  Test matrix vector multiplication y=Ax.
 *           Matrix A is a square matrix of size nxn.
 *           Column vectors x and y are of size nx1
 *
 * Input:    A[i][j]=c in all positions.  y[i] is 0 in all positions
 *           x[i]= i for 0<=i<n
 *
 * Note:     For simplicity, we assume n is divisible by no_proc
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "minunit.h"
#include "it_mult_vec.h"

#define MAX_TEST_MATRIX_SIZE 2048

#define FAIL 0
#define SUCC 1

#define TEST_CORRECTNESS 1

const double ERROR = 0.0001;

void print_error(const char *msgheader, const char *msg){
  printf("%s error: %s\n", msgheader, msg);
}

/*-------------------------------------------------------------------
 * Initialize test matrix and vectors.
 *   vector x of size n:    0 for every element
 *   vector d of size n:    (2n-1)/n for every element
 *
 * When matrix_type is not UPPER_TRIANGULAR
 *   matrix A of size nxn:  A[i,i]=0 for diagnal elements;
 *                          A[i,j]=-1/n for non-diagonal elements
 * When matrix_type is UPPER_TRIANGULAR
 *   matrix A of size nxn:  A[i,i]=0 for diagnal elements;
 *                          A[i,j]=-1/n for upper diagonal elements with i<j,
 *                          all lower triangular elements are 0
 * In args:
 *   n is the number of columns (and rows)
 *   marix_type: matrix type UPPER_TRIANGULAR or not
 *
 * Return value:
 *   If failed, return FAIL (0)
 *   If successful, return SUCC (1)
 */
int init_matrix(
    float *x, float *y, float *d, float *A, float *diff, int n, int matrix_type)
{
  int i, j;

  if (x == NULL || y == NULL || d==NULL
      ||  A == NULL || diff == NULL || n<=0) return FAIL;

  for (i = 0; i < n; i++) {
    x[i]=0;
    if (matrix_type == UPPER_TRIANGULAR) {
      d[i] = (2.0 * n - 1.0 * i - 1.0) / n;
    } else {
      d[i] = (2.0 * n - 1.0) / n;
    }

    for (j = 0; j < i; j++)
      if (matrix_type == UPPER_TRIANGULAR) {
        A[i * n + j] = 0.0;
      } else {
        A[i * n + j] = -1.0 / n;
      }

    A[i * n + i] = 0.0;

    for (j = i + 1; j < n; j++) {
        A[i * n + j] = -1.0 / n;
    }
  }

  return SUCC;
}

/*-----------------------------------
 * Validate the correctness of iterative computation
 * For the non-asynchrous mode, we use the Jacobi method, and thus you can use any thread configuration
 * For the asynchrous mode, we use the Gauss-Seidel method, you have to use 1x1 thread  configuration.
 *           Namely one block with 1 thread. With asychnous Gauss-Seidel running on multiple threads,
 *           the update speed may be inconsistent from one run to another run.
 */
 
const char*  validate_vect(
  const char *msgheader, float *actual_y, int n, int t, int matrix_type, int use_async)
{
  int i;
  if(n <= 0 )
    return "Failed: 0 or negative size";
  if(n > MAX_TEST_MATRIX_SIZE)
    return "Failed: Too big to validate";

  // Calculate expected.
  float *A, *x, *d, *y, *diff;

  A = (float*)malloc(n*n*sizeof(float));
  x = (float*)malloc(n*sizeof(float));
  d = (float*)malloc(n*sizeof(float));
  y = (float*)malloc(n*sizeof(float));
  diff = (float*)malloc(n*sizeof(float));
  init_matrix( x, y, d, A, diff, n, matrix_type);
  if(use_async)
    gsit_mult_vec_seq(n, y, d, A, x, matrix_type, t);
  else
    it_mult_vec_seq(n, y, d, A, x, matrix_type, t);

  for (i = 0; i < n; i++){
#ifdef DEBUG1
    printf("%s i=%d  Expected %f Actual %f\n", msgheader, i, y[i], actual_y[i]);
#endif
    mu_assert(
      "One mismatch in iterative mat-vect multiplication",
      fabs(y[i] - actual_y[i]) <= ERROR );
  }

  free(A);
  free(x);
  free(y);
  free(d);
  free(diff);
  return NULL;
}

/*-------------------------------------------------------------------
 * Test matrix vector multiplication
 * Process 0 collects the  error detection. If failed, return a message string
 * If successful, return NULL
 */
const char * itmv_test(
  const char *testmsg, int test_correctness, int n, int matrix_type, int t,
  int num_blocks, int threads_per_block, int use_async, int use_shared_x)
{
  float *A, *x, *d, *y, *diff;
  const char *msg;
  int i;

  A = (float*)malloc(n*n*sizeof(float));
  x = (float*)malloc(n*sizeof(float));
  d = (float*)malloc(n*sizeof(float));
  y = (float*)malloc(n*sizeof(float));
  diff = (float*)malloc(n*sizeof(float));
  init_matrix( x, y, d, A, diff, n, matrix_type);

  double tBefore = get_time();
  int no_iter= it_mult_vec(n, num_blocks, threads_per_block,
      y, d, A, x, diff, t, use_async, use_shared_x);
  double tAfter = get_time();

  printf("\n%s:", testmsg);
  printf("\nWith totally %d*%d threads, matrix size being %d, t being %d\n",
         num_blocks, threads_per_block, n, t);
  printf("Time cost in seconds: %f\n", tAfter - tBefore);

  float max_error = 0;
  for (i = 0; i < n; i++) 
    if (max_error < diff[i]) 
      max_error = diff[i];
  printf("Final error (|y-x|): %f.\n", max_error);
  printf("# of iterations executed: %d.\n", no_iter);
  if(no_iter<t) { 
    printf("Early exit due to convergence, even asked for %d iterations.\n", t);
    if(use_async)
      printf("Asynchronous code actually runs %d iterations.\n", no_iter);
  }
  printf("Final y[0]=%f. y[n-1]=%f\n", y[0], y[n-1]);

  msg = NULL;
  if (test_correctness == TEST_CORRECTNESS){
    msg = validate_vect(testmsg, y, n, no_iter, matrix_type, use_async);

    if (msg != NULL) print_error(testmsg, msg);
  }

  free(A);
  free(x);
  free(y);
  free(d);
  free(diff);

  return msg;
}

const char * itmv_test1() {
  return itmv_test(
      "Test 1:n=4, t=1, 1x2 threads", TEST_CORRECTNESS, 4, !UPPER_TRIANGULAR, 1, 1, 2,
      !USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test2() {
  return itmv_test(
      "Test 2:n=4, t=2, 1x2 threads", TEST_CORRECTNESS, 4, !UPPER_TRIANGULAR, 2, 1, 2,
      !USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test3() {
  return itmv_test(
      "Test 3:n=8, t=1, 1x2 threads", TEST_CORRECTNESS, 8, !UPPER_TRIANGULAR, 1, 1, 2,
      !USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test4() {
  return itmv_test(
      "Test 4:n=8, t=2, 1x2 threads", TEST_CORRECTNESS, 8, !UPPER_TRIANGULAR, 2, 1, 2,
      !USE_ASYNC, !USE_SHARED_X);
}


const char * itmv_test8a() {
  return itmv_test(
      "Test 8a:n=4, t=1, 1x1 threads/Gauss-Seidel", TEST_CORRECTNESS, 4, !UPPER_TRIANGULAR, 1, 1, 1,
      USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test8b() {
  return itmv_test(
      "Test 8b:n=4, t=2, 1x1 threads/Gauss-Seidel", TEST_CORRECTNESS, 4, !UPPER_TRIANGULAR, 2, 1, 1,
      USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test8c() {
  return itmv_test(
      "Test 8c:n=8, t=1, 1x1 threads/Gauss-Seidel", TEST_CORRECTNESS, 8, !UPPER_TRIANGULAR, 1, 1, 1,
      USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test8d() {
  return itmv_test(
      "Test 8d:n=8, t=2, 1x1 threads/Gauss-Seidel", TEST_CORRECTNESS, 8, !UPPER_TRIANGULAR, 2, 1, 1,
      USE_ASYNC, !USE_SHARED_X);
}

const char * itmv_test9() {
  return itmv_test(
      "Test 9: n=4K t=1K 32x128 threads", !TEST_CORRECTNESS,
      4096, !UPPER_TRIANGULAR, 1024, 1<<5, 1<<7, !USE_ASYNC, !USE_SHARED_X);
}

const char * itmv_test9a() {
  return itmv_test(
      "Test 9a: n=4K t=1K 16x128 threads", !TEST_CORRECTNESS,
      4096, !UPPER_TRIANGULAR, 1024, 1<<4, 1<<7, !USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test9b() {
  return itmv_test(
      "Test 9b: n=4K t=1K 8x128 threads", !TEST_CORRECTNESS,
      4096, !UPPER_TRIANGULAR, 1024, 1<<3, 1<<7, !USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test9c() {
  return itmv_test(
      "Test 9c: n=4K t=1K 4x128 threads", !TEST_CORRECTNESS,
      4096, !UPPER_TRIANGULAR, 1024, 1<<2, 1<<7, !USE_ASYNC, !USE_SHARED_X);
}

const char * itmv_test10() {
  return itmv_test(
      "Test 10: n=4K t=1K 32x128 threads/shared mem",
      !TEST_CORRECTNESS, 4096, !UPPER_TRIANGULAR, 1024, 1<<5, 1<<7,
      !USE_ASYNC, USE_SHARED_X);
}
const char * itmv_test10a() {
  return itmv_test(
      "Test 10a: n=4K t=1K 8x128 threads/shared mem",
      !TEST_CORRECTNESS, 4096, !UPPER_TRIANGULAR, 1024, 1<<3, 1<<7,
      !USE_ASYNC, USE_SHARED_X);
}

const char * itmv_test11() {
  return itmv_test(
      "Test 11: n=4K t=1K 32x128 threads/Async",
      !TEST_CORRECTNESS, 4096, !UPPER_TRIANGULAR, 1024, 1<<5, 1<<7,
      USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test11a() {
  return itmv_test(
      "Test 11a: n=4K t=1K 8x128 threads/Async",
      !TEST_CORRECTNESS, 4096, !UPPER_TRIANGULAR, 1024, 1<<3, 1<<7,
      USE_ASYNC, !USE_SHARED_X);
}

const char * itmv_test12() {
  return itmv_test(
      "Test 11: n=4K t=1K 32x128 threads/Async",
      TEST_CORRECTNESS, 4096, !UPPER_TRIANGULAR, 1024, 1<<5, 1<<7,
      USE_ASYNC, !USE_SHARED_X);
}
const char * itmv_test12a() {
  return itmv_test(
      "Test 11a: n=4K t=1K 8x128 threads/Async",
      TEST_CORRECTNESS, 4096, !UPPER_TRIANGULAR, 1024, 1<<3, 1<<7,
      USE_ASYNC, !USE_SHARED_X);
}


/*-------------------------------------------------------------------
 * Run all basic tests.  
 */
void run_basic_tests(void){
  printf(">>>>>>>>>>>>>>>>>>>>>>>>>\n");
  printf("Start running itmv tests.\n");
  printf(">>>>>>>>>>>>>>>>>>>>>>>>>\n");

  /*
   * Basic correctness tests without shared memory
   */
  mu_run_test(itmv_test1);
  mu_run_test(itmv_test2);
  mu_run_test(itmv_test3);
  mu_run_test(itmv_test4);


  /*
   * Basic correctness tests with async mode under 1x1 thread config. 
   */
  mu_run_test(itmv_test8a);
  mu_run_test(itmv_test8b);
  mu_run_test(itmv_test8c);
  mu_run_test(itmv_test8d);

}

/*---------------------------------------------------------------------
 * Run tests for larger matrices. 
 * You should call only when your basic tests succeed. 
 */
void run_large_matrix_tests(void){
  /*
   * Large matrix tests without using shared memory
   */
  mu_run_test(itmv_test9);
  mu_run_test(itmv_test9a);
  mu_run_test(itmv_test9b);
  mu_run_test(itmv_test9c); 
  

  /*
   * Large matrix tests with asynchronous mode.
   */
  mu_run_test(itmv_test11);
  mu_run_test(itmv_test11a);
}

void run_large_matrix_tests_with_seq(void) {
  mu_run_test(itmv_test12);
  mu_run_test(itmv_test12a);
}

/*-------------------------------------------------------------------
 * The main entrance to run all tests.
 * Only Proc 0 prints the test summary
 */
int main(){

  run_basic_tests();
  /*You should call large matrix tests only after passing your basic tests.*/
  /*run_large_matrix_tests();*/

  // run_large_matrix_tests_with_seq();

  mu_print_test_summary("\nSummary:");
}
