#!/bin/bash  
#SBATCH --job-name="treesum"  
#SBATCH --output="job_treesum4.%j.out"  
#SBATCH --partition=shared  
#SBATCH --account=csb175
#SBATCH --nodes=1  
#SBATCH --ntasks-per-node=4
#Above two lines specify total number of MPI processes= 1*4=4
#This job runs with 1 node, 4 cores per node for a total of 4 cores.  
#Next line sets 1 thread inside each process
#SBATCH --cpus-per-task=1
#SBATCH --export=ALL  
#SBATCH -t 00:03:00  

#ibrun in verbose mode will give binding detail  


#ibrun in verbose mode will give binding detail. It uses one MPI process per core.
#it is the same as "srun --mpi=pmi2 -n 4 ../hello1_mpi"

module purge
module load slurm
module load cpu
module load gcc/10.2.0 openmpi/4.1.1

module load sdsc

ibrun -v ./treesum_test_mpi
