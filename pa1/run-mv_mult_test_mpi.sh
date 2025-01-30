#!/bin/bash  
# Next line shows the job name you can find when querying the job status
#SBATCH --job-name="mv_mpi"  
# Next line is the output file name of the execution log
#SBATCH --output="job_mv_4nodes.%j.out"  
# Next line shows where to ask for machine nodes
#SBATCH --partition=shared  
#This job runs with 1 node, 4 cores per node for a total of 4 cores.  
#Total number of MPI processes= 1*4=4
#SBATCH --nodes=1  
#SBATCH --ntasks-per-node=4

#Next line sets 1 thread inside each process
#SBATCH --cpus-per-task=1

#SBATCH --export=ALL  
# Next line limits the job execution time at most 3 minutes.
#SBATCH -t 00:03:00  


#SBATCH --account=csb175
#csb175 is the class-wide account shared among all cs140 students


#ibrun in verbose mode will give binding detail. It uses one MPI process per core.


module purge
module load slurm
module load cpu
#module load gcc
#module load openmpi

module load gcc/10.2.0
module load openmpi/4.1.1
module load sdsc

#ibrun -v ./mv_mult_test_mpi  
srun --mpi=pmi2 -n 4 ./mv_mult_test_mpi  
