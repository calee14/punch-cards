#!/bin/bash  
# Next line shows the job name you can find when querying the job status
#SBATCH --job-name="pimc_pth"
# Next line is the output file name of the execution log
#SBATCH --output="job_pimc_pth_4core.%j.out"
# Next line shows where to ask for machine nodes
#SBATCH --partition=shared
#Next line asks for 1 node and  4 cores per node for a total of 4 cores.
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --export=ALL
# Next line limits the job execution time at most 4 minute.
#SBATCH -t 00:04:00
#SBATCH --account=csb175


./cs140barrier_test
