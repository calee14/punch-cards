Last name of Student 1: Lee
First name of Student 1: Cappillen
Email of Student 1: cappillen@ucsb.edu
GradeScope account name of Student 1: cappillen
Last name of Student 2:
First name of Student 2:
Email of Student 2:
GradeScope account name of Student 2: 


----------------------------------------------------------------------------
Report for Question 1 

List your code change for this question 
Changes: ran large tests by invoking the 'run_large_matrix_tests' function

Parallel time for n=4K, t=1K,  4x128  threads
Time cost in seconds: 3.931096 (threads)

Parallel time for n=4K, t=1K,  8x128  threads
Time cost in seconds: 1.972846 (threads)
Time cost in seconds: 0.040250 (async)

Parallel time for n=4K, t=1K,  16x128 threads
Time cost in seconds: 0.996064 (threads)

Parallel time for n=4K, t=1K,  32x128 threads
Time cost in seconds: 0.532523 (threads)
Time cost in seconds: 0.060423 (async)


Do you see a trend of  speedup improvement  with more threads? We expect a good speedup and explain the reason.
Yes, there is a trend of speedup imporvment when we add more threads. When we scale up the number of threads by a factor the parallel time scales down by around that same factor. When the parallel time decreases then we know that the speedup ratio is also improving, too.

----------------------------------------------------------------------------
Report for Question 2 

Removed due to the change of the assignment
----------------------------------------------------------------------------
Report for Question 3 
List your code change for this question
Changes: ran large tests by invoking the 'run_large_matrix_tests' function


Let the default number of asynchronous iterations be 5 in a batch as specified in it_mult_vec.h.
List reported parallel time and the number of actual iterations executed  for n=4K, t=1K, 8x128  threads with asynchronous Gauss Seidel
Time cost in seconds: 0.040250
# of iterations executed: 15.

List reported parallel time and the number of actual iterations executed  for n=4K, t=1K,  32x128 threads with asynchronous Gauss Seidel
Time cost in seconds: 0.060423
# of iterations executed: 105.

Is the number of iterations  executed by  above parallel asynchronous Gauss Seidel-Seidel method  bigger or smaller  than that
of the sequential Gauss Seidel-Seidel code under the same converging error threshold (1e-3)?  
Explain the reason based on the running trace of above two thread configurations that more threads may not yield more time reduction in this case. 




----------------------------------------------------------------------------
Attach the  output trace  of your code below in running the tests of the unmodified it_mult_vec_test.cu on Expanse GPU
>>>>>>>>>>>>>>>>>>>>>>>>>
Start running itmv tests.
>>>>>>>>>>>>>>>>>>>>>>>>>

Test 1:n=4, t=1, 1x2 threads:
With totally 1*2 threads, matrix size being 4, t being 1
Time cost in seconds: 0.150747
Final error (|y-x|): 1.750000.
# of iterations executed: 1.
Final y[0]=1.750000. y[n-1]=1.750000

Test 2:n=4, t=2, 1x2 threads:
With totally 1*2 threads, matrix size being 4, t being 2
Time cost in seconds: 0.000265
Final error (|y-x|): 1.312500.
# of iterations executed: 2.
Final y[0]=0.437500. y[n-1]=0.437500

Test 3:n=8, t=1, 1x2 threads:
With totally 1*2 threads, matrix size being 8, t being 1
Time cost in seconds: 0.000241
Final error (|y-x|): 1.875000.
# of iterations executed: 1.
Final y[0]=1.875000. y[n-1]=1.875000

Test 4:n=8, t=2, 1x2 threads:
With totally 1*2 threads, matrix size being 8, t being 2
Time cost in seconds: 0.000250
Final error (|y-x|): 1.640625.
# of iterations executed: 2.
Final y[0]=0.234375. y[n-1]=0.234375

Test 8a:n=4, t=1, 1x1 threads/Gauss-Seidel:
With totally 1*1 threads, matrix size being 4, t being 1
Time cost in seconds: 0.000242
Final error (|y-x|): 1.000193.
# of iterations executed: 5.
Final y[0]=1.000089. y[n-1]=1.000193

Test 8b:n=4, t=2, 1x1 threads/Gauss-Seidel:
With totally 1*1 threads, matrix size being 4, t being 2
Time cost in seconds: 0.000237
Final error (|y-x|): 1.000193.
# of iterations executed: 5.
Final y[0]=1.000089. y[n-1]=1.000193

Test 8c:n=8, t=1, 1x1 threads/Gauss-Seidel:
With totally 1*1 threads, matrix size being 8, t being 1
Time cost in seconds: 0.000252
Final error (|y-x|): 1.001155.
# of iterations executed: 5.
Final y[0]=1.001155. y[n-1]=0.999790

Test 8d:n=8, t=2, 1x1 threads/Gauss-Seidel:
With totally 1*1 threads, matrix size being 8, t being 2
Time cost in seconds: 0.000240
Final error (|y-x|): 1.001155.
# of iterations executed: 5.
Final y[0]=1.001155. y[n-1]=0.999790

Test 9: n=4K t=1K 32x128 threads:
With totally 32*128 threads, matrix size being 4096, t being 1024
Time cost in seconds: 0.532523
Final error (|y-x|): 1.557740.
# of iterations executed: 1024.
Final y[0]=0.221225. y[n-1]=0.221225

Test 9a: n=4K t=1K 16x128 threads:
With totally 16*128 threads, matrix size being 4096, t being 1024
Time cost in seconds: 0.996064
Final error (|y-x|): 1.557740.
# of iterations executed: 1024.
Final y[0]=0.221225. y[n-1]=0.221225

Test 9b: n=4K t=1K 8x128 threads:
With totally 8*128 threads, matrix size being 4096, t being 1024
Time cost in seconds: 1.972846
Final error (|y-x|): 1.557740.
# of iterations executed: 1024.
Final y[0]=0.221225. y[n-1]=0.221225

Test 9c: n=4K t=1K 4x128 threads:
With totally 4*128 threads, matrix size being 4096, t being 1024
Time cost in seconds: 3.931096
Final error (|y-x|): 1.557740.
# of iterations executed: 1024.
Final y[0]=0.221225. y[n-1]=0.221225

Test 11: n=4K t=1K 32x128 threads/Async:
With totally 32*128 threads, matrix size being 4096, t being 1024
Time cost in seconds: 0.060423
Final error (|y-x|): 0.000748.
# of iterations executed: 105.
Early exit due to convergence, even asked for 1024 iterations.
Asynchronous code actually runs 105 iterations.
Final y[0]=1.000311. y[n-1]=1.000307

Test 11a: n=4K t=1K 8x128 threads/Async:
With totally 8*128 threads, matrix size being 4096, t being 1024
Time cost in seconds: 0.040250
Final error (|y-x|): 0.000026.
# of iterations executed: 15.
Early exit due to convergence, even asked for 1024 iterations.
Asynchronous code actually runs 15 iterations.
Final y[0]=1.000000. y[n-1]=1.000000

Summary: Failed 0 out of 14 tests