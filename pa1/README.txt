Last name of Student 1: Lee
First name of Student 1: Cappillen
Email of Student 1: cappillen@ucsb.edu
Last name of Student 2:
First name of Student 2:
Email of Student 2:



Report for Question 2.a
Sequential time: 54.832832 seconds

Parallel time (2 cores): 20.776970 seconds
Speed up (2 cores): 1.873
Efficiency (2 cores): 0.936 

Parallel time (4 cores): 14.802262 seconds
Speed up (4 cores): 3.704
Efficiency (4 cores): 0.926 

Report for Question 2.b
Sequential time: 28.274537 seconds

Parallel time (2 cores): 22.347767 seconds
Speed up (2 cores): 1.265
Efficiency (2 cores): 0.6326 

Parallel time (4 cores): 13.065968 seconds
Speed up (4 cores): 2.164
Efficiency (4 cores): 0.5409

The efficiency is lower because the workload isn't being evenly load balanced. For some processors' tasks they have simple and fast computations because they can skip a lot of the work due the property of the upper triangle matrix. Using sequential blocks to partition data means that some processors will have hard tasks to compute whereas some processors can skip a lot of the computation because of the upper triangle properties. A way we can fix this is to use a cyclic or cyclic-block partition of the data. This way the workload is evenly distributed. 

