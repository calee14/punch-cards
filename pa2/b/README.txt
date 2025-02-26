Last name of Student 1: Cappillen
First name of Student 1: Lee
Email of Student 1: cappillen@ucsb.edu
Last name of Student 2:
First name of Student 2:
Email of Student 2:

Q2.a List parallel code that uses at most two barrier calls inside the while loop

The word_block() uses only two barrier calls inside the t iterations loop. You can see them around lines 96 and 103.
The work_blockcyclic() uses only two barrier calls inside the t iterations loop. You can see them around lines 145 and 156.


Q2.b Report parallel time, speedup, and efficiency for  the upper triangular test matrix case when n=4096 and t=1024. 
Use 2 threads and 4  threads (1 thread per core) under blocking mapping, and block cyclic mapping with block size 1 and block size 16.    

For 2 threads: 
Block-mapping speedup: 1.31
Block-mapping efficiency: 0.657
Cyclic-block (block_size=1) speedup: 1.90
Cyclic-block (block_size=1) efficiency: 0.952
Cyclic-block (block_size=16) speedup: 1.92
Cyclic-block (block_size=16) speedup: 0.961

For 4 threads:
Block-mapping speedup: 2.31
Block-mapping efficiency: 0.577
Cyclic-block (block_size=1) speedup: 3.895
Cyclic-block (block_size=1) efficiency: 0.974
Cyclic-block (block_size=16) speedup: 3.845
Cyclic-block (block_size=16) speedup: 0.961



Please indicate if your evaluation is done on CSIL and if yes, list the uptime index of that CSIL machine.  
Yes, I used CSIL because Expanse was down.
For 2 cores uptime index (ran uptime command immediately after program finished in linux terminal)
13:16:36 up  8:16,  7 users,  load average: 1.06, 0.62, 0.45

For 4 cores uptime index (ran uptime command immediately after program finished in linux terminal)
13:15:44 up  8:15,  7 users,  load average: 0.92, 0.53, 0.41
