#!/bin/sh
#BATCH --job-name="it_mult_vec"
#SBATCH --output="it_mult_vec.%j.out"
#SBATCH --partition=gpu-shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --gpus=1
#SBATCH --export=ALL
#SBATCH -t 00:03:00
#SBATCH --account=csb175

#Environment for the CUDA 
module purge
module load slurm
#module load gpu
#module load cuda
module load gpu/0.15.4 gcc/7.2.0 cuda/11.0.2
./it_mult_vec_test
