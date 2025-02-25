#!/bin/bash  
# Next line shows the job name you can find when querying the job status
#SBATCH --job-name="itmvpth"
# Next line is the output file name of the execution log
#SBATCH --output="job_itmvpth_4.%j.out"
# Next line shows where to ask for machine nodes
#SBATCH --partition=shared
#Next line asks for 1 machine node and  4 core per node 
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --export=ALL
# Next line limits the job execution time at most 3 minute.
#SBATCH -t 00:03:00
#SBATCH --account=csb175


./itmv_mult_test_pth 1
./itmv_mult_test_pth 2
./itmv_mult_test_pth 4
