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


Parallel time for n=4K, t=1K,  4x128  threads

Parallel time for n=4K, t=1K,  8x128  threads

Parallel time for n=4K, t=1K,  16x128 threads

Parallel time for n=4K, t=1K,  32x128 threads


Do you see a trend of  speedup improvement  with more threads? We expect a good speedup and explain the reason.


----------------------------------------------------------------------------
Report for Question 2 

Removed due to the change of the assignment
----------------------------------------------------------------------------
Report for Question 3 
List your code change for this question




Let the default number of asynchronous iterations be 5 in a batch as specified in it_mult_vec.h.
List reported parallel time and the number of actual iterations executed  for n=4K, t=1K, 8x128  threads with asynchronous Gauss Seidel


List reported parallel time and the number of actual iterations executed  for n=4K, t=1K,  32x128 threads with asynchronous Gauss Seidel


Is the number of iterations  executed by  above parallel asynchronous Gauss Seidel-Seidel method  bigger or smaller  than that
of the sequential Gauss Seidel-Seidel code under the same converging error threshold (1e-3)?  
Explain the reason based on the running trace of above two thread configurations that more threads may not yield more time reduction in this case. 




----------------------------------------------------------------------------
Attach the  output trace  of your code below in running the tests of the unmodified it_mult_vec_test.cu on Expanse GPU
