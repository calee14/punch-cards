/*
 * File: it_mult_vec.cu
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "it_mult_vec.h"

/*-------------------------------------------------------------------
 * Function:    mult_vec_async
 * Purpose:     Run in asynchronous Gauss-Seidel style {y=d+Ay} on GPU for multiple iterations.
 * In args:     A:  matrix A
 *              d:  column vector d
 *              x:  column vector x as the initial solution vector 
 *              n:  the global  number of columns (same as the number of rows)
 *              rows_per_thread: the number of rows for each thread
 *
 * Out args:    y: final column solution vector 
 */
__global__
void mult_vec_async(int n, int rows_per_thread, int num_async_iter, float *y, float *d,
                    float *A, float *x, float *diff) {

#ifdef DEBUG1
  dprint_sample ( "GS GPU ", A,  x, d, y, n, num_async_iter, !UPPER_TRIANGULAR);
#endif
  int idx=0; /*Assign a linearized thread ID, so I can be responsible for some rows*/
  /*Your solution to compute idx */
  idx = blockIdx.x * blockDim.x + threadIdx.x;
  
  for (int i = 0; i < rows_per_thread; i++) {
    int row_index = idx * rows_per_thread + i;
    y[row_index] = x[row_index]; //Start with current value of x
  }
  for (int k = 0; k < num_async_iter; k++) {
    /*Perform asynchronous Gauss-Seidel method for y=d+Ay*/
    /*Your solution*/
    


#ifdef DEBUG1
    dprint_samplexy ( "GS GPU ", k, x, y, n);
#endif
  }
 
  for (int i = 0; i < rows_per_thread; i++) {
    int row_index = idx * rows_per_thread + i;
    diff[row_index] = fabs(x[row_index] - y[row_index]); //Compute the difference
  }
  
}


/*-------------------------------------------------------------------
 * Function:  mult_vec
 * Purpose:   Run a single Jacobi iteration {y=d+Ax} on the GPU.
 * In args:   A:  matrix A
 *            d:  column vector d
 *            n:  the global  number of columns (same as the number of rows)
 *            rows_per_thread: the number of rows for each thread
 *            x: column vector x
 * Out args:  y: column vector y
 * Return:    void
 */
__global__
void mult_vec(int n, int rows_per_thread, float *y, float *d, float *A,
              float *x, float *diff) {
  int idx=0; /*Assign a linearized thread ID, which will be used to determine what I own*/ 
  /*Your solution to compute idx */ 
  idx = blockIdx.x * blockDim.x + threadIdx.x;

  for (int i = 0; i < rows_per_thread; i++) {
    int row_index = idx * rows_per_thread + i;
    double sum = d[row_index];
    for (int j = 0; j < n; j++) {
      sum += A[row_index*n + j]*x[j];
    }
    y[row_index] = sum;

    diff[row_index] = fabs(sum - x[row_index]);
  }
}

/*-------------------------------------------------------------------
 * Function:  it_mult_vec
 * Purpose:   Run t iterations of  computation:  {y=d+Ax} on the GPU, swap x and y after each call.
 *            You can assume N/(num_blocks *threads_per_block) is an integer
 * In args:   A:  matrix A
 *            d:  column vector d
 *            N:  the global  number of columns (same as the number of rows)
 *            t:  the number of iterations
 *            num_blocks: number of blocks
 *            threads_per_block: number of threads per block
 * In/out:    x:    column vector x   Contain the initial solution vector, revised iteratively
 *            y:    column vector y   Final solution vector
 *            diff: vector of element-wise difference between x and y
 * Return:    If return is positive, successfully finish, return # of iterations executed
 *            If return is -1, it means there is at least one invalid input or some execution error
 *
 */
int it_mult_vec(int N,
                int num_blocks,
                int threads_per_block,
                float *y,
                float *d,
                float *A,
                float *x,
                float *diff,
                int iterations,
                int use_async,
                int use_shared_x) {
  if (y == 0 || d == 0 || A == 0 || x == 0 || diff == 0) return -1;
  if (num_blocks * threads_per_block > N) {
    printf("The number of total threads is larger than the matrix size N.\n");
    return -1;
  }
  // Check if  N can be   divided by threads_per_block.
  if (N % (num_blocks*threads_per_block)) {
    printf("The matrix size N should be divisible by num_blocks*threads_per_block.\n");
    return -1;
  }

  // *_d are pointers for memory in device.
  float *A_d, *x_d, *y_d, *d_d, *diff_d;
  int k, j, result, reach_converge;
  int row_size = N * sizeof(float);
  int A_size = N * row_size;

  /*Allocate device global space for matrix A. Copy  data to the device global memory*/
  /*Your solution*/
  result = cudaMalloc((void **) &A_d, A_size);
  if (result) {
    printf("Errorr in cudaMalloc. Error code is %d.\n", result);
    return -1;
  }
  result = cudaMemcpy(A_d, A, A_size, cudaMemcpyHostToDevice);
  if (result) {
    print("Error in cudaMemcpy. Error code is %d.\n", result);
    return -1
  }
  
  /*Allocate, and copy other  data to the device global memory*/
  result = cudaMalloc( (void **) &x_d, row_size);
  if (result) {
    printf("Error in cudaMalloc. Error code is %d.\n", result);
    return -1;
  }
  result = cudaMemcpy(x_d, x, row_size, cudaMemcpyHostToDevice);
  if (result) {
    printf("Error in cudaMemcpy. Error code is %d.\n", result);
    return -1;
  }
  result = cudaMalloc( (void **) &d_d, row_size);
  if (result) {
    printf("Error in cudaMalloc. Error code is %d.\n", result);
    return -1;
  }
  result = cudaMemcpy(d_d, d, row_size, cudaMemcpyHostToDevice);
  if (result) {
    printf("Error in cudaMemcpy. Error code is %d.\n", result);
    return -1;
  } 
  result = cudaMalloc( (void **) &y_d, row_size);
  if (result) {
    printf("Error in cudaMalloc. Error code is %d.\n", result);
    return -1;
  }
  result = cudaMemcpy(y_d, x, row_size, cudaMemcpyHostToDevice);
  if (result) {
    printf("Error in cudaMemcpy for y's initial value. Error code is %d.\n", result);
    return -1;
  }
  result = cudaMalloc( (void **) &diff_d, row_size);
  if (result) {
    printf("Error in cudaMalloc. Error code is %d.\n", result);
    return -1;
  }
  /* You can assume N/num_blocks/threads_per_block is an integer*/
  int rows_per_thread = ceil(N * 1.0 / num_blocks / threads_per_block);
  k=0; 
  while (k < iterations) {
    if (use_async) {
      mult_vec_async<<<num_blocks, threads_per_block>>>(
          N, rows_per_thread, NUM_ASYNC_ITER, y_d, d_d, A_d, x_d, diff_d);

      k += NUM_ASYNC_ITER; //The above line already executes NUM_ASYNC_ITER iterations
    } else { /* call a kernel Jacobi method to compute y=d+Ax*/
      /*Your solution*/
      mult_vec<<<num_blocks, threads_per_block>>>(
        N, rows_per_thread, y_d, d_d, A_d, x_d, diff_d);
     k++;
    }
    // Detect convergence. Copy the difference vector from the device
    result = cudaMemcpy(diff, diff_d, row_size, cudaMemcpyDeviceToHost);
    if (result) {
      printf("Error in cudaMemcpy. Error code is %d.\n", result);
      return -1;
    }
    reach_converge = 1;
    for (j = 0; reach_converge && j < N; j++)
      reach_converge = (diff[j] <= CONVERGE_THRESHOLD);

    if (reach_converge) {
      break;
    }

    if (k  < iterations) { //Swap x and y pointers, so next round strts with latest solution
      float *tmp = x_d;
      x_d = y_d;
      y_d = tmp;
    }
  }
  /*Copy the final solution vector y from device. */
  /*Your solution*/
  result = cudaMemcpy(y, y_d, row_size, cudaMemcpyDeviceToHost);
  if (result) {
    printf("Error in cudaMemcpy. Error code is %d.\n", result);
    return -1;
  }

  cudaFree(A_d);
  cudaFree(x_d);
  cudaFree(y_d);
  cudaFree(d_d);
  cudaFree(diff_d);

  return k; //Actual number of iterations executed.
}

/*-------------------------------------------------------------------
 * Function:  it_mult_vec_seq
 * Purpose:   Run iterations of computation: {y=d+Ax; x=y} sequentially.
 *            Break if converge.
 * In args:   A:  matrix A
 *            d:  column vector d
 *            matrix_type:  matrix_type=0 means A is a regular matrix.
 *                          matrix_type=1 (UPPER_TRIANGULAR)
                            means A is an upper triangular matrix
 *            N:  the global  number of columns (same as the number of rows)
 *            iterations:   the number of iterations
 * In/out:    x:  column vector x
 *            y:  column vector y
 * Return:  1  means succesful
 *          0  means unsuccessful
 * Errors:  If an error is detected
 *          (e.g. n is non-positive, matrix/vector pointers are NULL)
 *
 */
int it_mult_vec_seq(int N,
                    float *y,
                    float *d,
                    float *A,
                    float *x,
                    int matrix_type,
                    int iterations) {
  int i, j, start, k, reach_converge;

  if (N <= 0 || A == NULL || x == NULL || d == NULL || y == NULL)
    return 0;

  for (k = 0; k < iterations; k++) {
    for (i = 0; i < N; i++) {
      y[i] = d[i];
      if (matrix_type == UPPER_TRIANGULAR) {
        start = i;
      } else {
        start = 0;
      }
      for (j = start; j < N; j++) {
        y[i] += A[i*N+j]*x[j];
      }
    }

    reach_converge = 1;
    for (i = 0; i < N; i++) {
      reach_converge =
          fabs(y[i] - x[i]) > CONVERGE_THRESHOLD ? 0 : reach_converge;
      x[i] = y[i];
    }

    if (reach_converge) break;
  }
  return 1;
}
/*-------------------------------------------------------------------
 * Function:  gsit_mult_vec_seq
 * Purpose:   Run iterations of Gauss-Seidel method: {y=d+Ay} sequentially.
 *            Break if converge.
 * In args:   A:  matrix A
 *            d:  column vector d
 *            matrix_type:  matrix_type=0 means A is a regular matrix.
 *                          matrix_type=1 (UPPER_TRIANGULAR)
                            means A is an upper triangular matrix
 *            N:  the global  number of columns (same as the number of rows)
 *            iterations:   the number of iterations
 * In/out:    x:  column vector x  initial solution
 *            y:  column vector y  final solution
 * Return:  1  means succesful
 *          0  means unsuccessful
 * Errors:  If an error is detected
 *          (e.g. n is non-positive, matrix/vector pointers are NULL)
 *
 */
int gsit_mult_vec_seq(int N,
                    float *y,
                    float *d,
                    float *A,
                    float *x,
                    int matrix_type,
                    int iterations) {
  int i, j, start, k, reach_converge;

  if (N <= 0 || A == NULL || x == NULL || d == NULL || y == NULL)
    return 0;
#ifdef DEBUG1
  print_sample ( "GS host ", A,  x, d, y, N, iterations, matrix_type);
#endif
  for (i = 0; i < N; i++) {//initialize with x
    y[i] = x[i];
  }
  for (k = 0; k < iterations; k++) {
    for (i = 0; i < N; i++) {
      float sum= d[i];
      if (matrix_type == UPPER_TRIANGULAR) {
        start = i;
      } else {
        start = 0;
      }
      for (j = start; j < N; j++) {
        sum += A[i*N+j]*y[j];
      }
      y[i]=sum;
    }

    reach_converge = 1;
    for (i = 0; i < N; i++) {
      reach_converge =
          fabs(y[i] - x[i]) > CONVERGE_THRESHOLD ? 0 : reach_converge;
    }

    if (reach_converge) break;
    for (i = 0; i < N; i++) {//remember last version 
      x[i] = y[i];
    }
#ifdef DEBUG1
    print_samplexy ( "GS host ", k, x, y, N);
#endif
  }
  return 1;
}

/*
 The following functions are useful for debugging.
 */
void print_sample ( const char* msgheader, float A[],  float x[], float d[], float  y[], int n, int t, int matrix_type) {
  printf("%s Test matrix type %d, size n=%d, t=%d\n", msgheader, matrix_type,n, t);
  if(n<4 || A==NULL || x==NULL ||   d==NULL|| y==NULL)
    return;
  printf("%s check x[0-3] %f, %f, %f, %f\n", msgheader, x[0], x[1], x[2], x[3]);
  printf("%s check y[0-3] %f, %f, %f, %f\n", msgheader, y[0], y[1], y[2], y[3]);
  printf("%s check d[0-3] are %f, %f, %f, %f\n", msgheader, d[0], d[1], d[2], d[3]);
  printf("%s check A[0][0-3] are %f, %f, %f, %f\n", msgheader, A[0], A[1], A[2], A[3]);
  printf("%s check A[1][0-3] are %f, %f, %f, %f\n", msgheader, A[n], A[n+1], A[n+2], A[n+3]);
  printf("%s check A[2][0-3] are %f, %f, %f, %f\n", msgheader, A[2*n], A[2*n+1], A[2*n+2], A[2*n+3]);
  printf("%s check A[3][0-3] are %f, %f, %f, %f\n", msgheader,  A[3*n], A[3*n+1], A[3*n+2], A[3*n+3]);

}

__device__ void dprint_sample ( const char* msgheader, float A[],  float x[], float d[], float  y[], int n, int t, int matrix_type) {
  printf("%s Test matrix type %d, size n=%d, t=%d\n", msgheader, matrix_type,n, t);
  if(n<4 || A==NULL || x==NULL ||   d==NULL|| y==NULL)
    return;
  printf("%s check x[0-3] %f, %f, %f, %f\n", msgheader, x[0], x[1], x[2], x[3]);
  printf("%s check y[0-3] %f, %f, %f, %f\n", msgheader, y[0], y[1], y[2], y[3]);
  printf("%s check d[0-3] are %f, %f, %f, %f\n", msgheader, d[0], d[1], d[2], d[3]);
  printf("%s check A[0][0-3] are %f, %f, %f, %f\n", msgheader, A[0], A[1], A[2], A[3]);
  printf("%s check A[1][0-3] are %f, %f, %f, %f\n", msgheader, A[n], A[n+1], A[n+2], A[n+3]);
  printf("%s check A[2][0-3] are %f, %f, %f, %f\n", msgheader, A[2*n], A[2*n+1], A[2*n+2], A[2*n+3]);
  printf("%s check A[3][0-3] are %f, %f, %f, %f\n", msgheader,  A[3*n], A[3*n+1], A[3*n+2], A[3*n+3]);

}
void print_samplexy ( const char* msgheader, int k, float x[], float y[], int n) {
  if(k>3|| n<4 || x==NULL ||   y==NULL) //No print if k is too big or n is too small 
    return;
  printf("%s %d check x[0-3] %f, %f, %f, %f\n",   msgheader, k, x[0], x[1], x[2], x[3]);
  printf("%s %d check y[0-3] %f, %f, %f, %f\n",   msgheader, k,y[0], y[1], y[2], y[3]);
}
__device__ void dprint_samplexy ( const char* msgheader, int k, float x[], float y[], int n) {
  if(n<4 || x==NULL ||   y==NULL)
    return;
  printf("%s %d check x[0-3] %f, %f, %f, %f\n",  msgheader,k, x[0], x[1], x[2], x[3]);
  printf("%s %d check y[0-3] %f, %f, %f, %f\n",  msgheader,k, y[0], y[1], y[2], y[3]);
}

