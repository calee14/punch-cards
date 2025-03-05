/*
 * File: it_mult_vec.h
 *
 */

#ifndef _IT_MULT_VEC_H
#define _IT_MULT_VEC_H

#include <curand.h>
#include <curand_kernel.h>

#define UPPER_TRIANGULAR 1
#define USE_ASYNC 1
#define USE_SHARED_X 1
#define SHARED_X_SIZE 1<<12
//#define NUM_ASYNC_ITER 31
#define NUM_ASYNC_ITER 5
#define CONVERGE_THRESHOLD 1e-3

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
                int use_shared_x);

int it_mult_vec_seq(int N,
                    float *y,
                    float *d,
                    float *A,
                    float *x,
                    int matrix_type,
                    int iterations);

int gsit_mult_vec_seq(int N,
                    float *y,
                    float *d,
                    float *A,
                    float *x,
                    int matrix_type,
                    int iterations);

void print_sample ( const char* msgheader, float A[],  float x[], float d[], float  y[], int n, int t, int matrix_type); 
__device__ void dprint_sample ( const char* msgheader, float A[],  float x[], 
	float d[], float  y[], int n, int t, int matrix_type); 
void print_samplexy ( const char* msgheader, int k, float x[], float y[], int n);
__device__ void dprint_samplexy ( const char* msgheader, int k, float x[], float y[], int n);

#endif
