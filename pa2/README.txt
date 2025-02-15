Last name of Student 1: Lee
First name of Student 1: Cappillen
Email of Student 1: cappillen@ucsb.edu
Last name of Student 2:
First name of Student 2:
Email of Student 2:


If CSIL is used for performance assessment instead of Expanse, make sure you evaluate when such a machine is lightly 
loaded using “uptime”. Please  indicate your evaluation is done on CSIL and list the uptime index of that CSIL machine.  

Report 
----------------------------------------------------------------------------
1. How is the code parallelized? Show your solution by listing the key computation parallelized with
  OpenMP and related code.
The code is parallelized and tested on Expanse. The key computation that's parallelized are the two for-loops. 
The first one calculates the computation of A_i * x + d.
#pragma omp parallel for private(i)
for (i = 0; i < matrix_dim; i++) {
    mv_compute(i);
}

The second for loop handles copying the values of y into x. 
#pragma omp parallel for private(i)
for (i = 0; i < matrix_dim; i++) {
    vector_x[i] = vector_y[i];
} 

Using the #pragma clause we can apply parallalization because it will create threads to process different parts of the for-loop computation in parallel.

----------------------------------------------------------------------------
2. Parallel time/speedup/efficiency using 2 and 4 cores under different scheduling/mapping methods
    for a full matrix (n=4096 and t=1024) on Expanse.
Sequential time (for 4 core test suite): 64.588332 seconds
Sequential time (for 2 core test suite): 63.866811 seconds 

Parallel time (2 cores) block-mapping: 31.849130 seconds
Parallel time (2 cores) block-cyclic, chunksize=1: 31.761897 seconds
Parallel time (2 cores) block-cyclic, chunksize=16: 32.034932 seconds
Parallel time (2 cores) block-dynamic, chunksize=16: 32.034932 seconds
Parallel time (4 cores) block-mapping: 16.663938 seconds
Parallel time (4 cores) block-cyclic, chunksize=1: 16.571280 seconds
Parallel time (4 cores) block-cyclic, chunksize=16: 16.571280 seconds
Parallel time (4 cores) block-dynamic, chunksize=16: 16.485888 seconds

Speedup (2 cores) block-mapping: 1.9997
Speedup (2 cores) block-cyclic, chunksize=1: 2.005
Speedup (2 cores) block-cyclic, chunksize=16: 1.996
Speedup (2 cores) block-dynamic, chunksize=16: 1.9936
Speedup (4 cores) block-mapping: 3.876
Speedup (4 cores) block-cyclic, chunksize=1: 3.897
Speedup (4 cores) block-cyclic, chunksize=16: 3.897
Speedup (4 cores) block-dynamic, chunksize=16: 3.917

Efficiency (2 cores) block-mapping: 0.99985
Efficiency (2 cores) block-cyclic, chunksize=1: 1.00 
Efficiency (2 cores) block-cyclic, chunksize=16: 0.998
Efficiency (2 cores) block-dynamic, chunksize=16: 0.9968 
Efficiency (4 cores) block-mapping: 0.969
Efficiency (4 cores) block-cyclic, chunksize=1: 0.974
Efficiency (4 cores) block-cyclic, chunksize=16: 0.974
Efficiency (4 cores) block-dynamic, chunksize=16: 0.979
----------------------------------------------------------------------------
3. Parallel time/speedup/efficiency using 2 and 4 cores under different scheduling/mapping methods
 for an upper triangular matrix (n=4096 and t=1024) on Expanse.
Explain  the above significant performance differences of the above methods with a short reason.
 
Sequential time (for 4 core test suite): 32.505699 seconds
Sequential time (for 2 core test suite): 31.866811 seconds 

Parallel time (2 cores) block-mapping: 24.361696 seconds
Parallel time (2 cores) block-cyclic, chunksize=1: 24.368425 seconds
Parallel time (2 cores) block-cyclic, chunksize=16: 24.432609 seconds
Parallel time (2 cores) block-dynamic, chunksize=16: 24.438078 seconds
Parallel time (4 cores) block-mapping: 14.366710 seconds
Parallel time (4 cores) block-cyclic, chunksize=1: 14.366710 seconds
Parallel time (4 cores) block-cyclic, chunksize=16: 14.642593 seconds
Parallel time (4 cores) block-dynamic, chunksize=16: 14.65809 seconds

Speedup (2 cores) block-mapping: 1.308
Speedup (2 cores) block-cyclic, chunksize=1: 1.307
Speedup (2 cores) block-cyclic, chunksize=16: 1.304
Speedup (2 cores) block-dynamic, chunksize=16: 1.304
Speedup (4 cores) block-mapping: 2.262
Speedup (4 cores) block-cyclic, chunksize=1: 2.262
Speedup (4 cores) block-cyclic, chunksize=16: 2.220
Speedup (4 cores) block-dynamic, chunksize=16: 2.218

Efficiency (2 cores) block-mapping: 0.654
Efficiency (2 cores) block-cyclic, chunksize=1: 0.654
Efficiency (2 cores) block-cyclic, chunksize=16: 0.652
Efficiency (2 cores) block-dynamic, chunksize=16: 0.652
Efficiency (4 cores) block-mapping: 0.566
Efficiency (4 cores) block-cyclic, chunksize=1: 0.566
Efficiency (4 cores) block-cyclic, chunksize=16: 0.555
Efficiency (4 cores) block-dynamic, chunksize=16: 0.554
