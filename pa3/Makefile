
#CC       = pgcc
CC        = icc
NVCC      = nvcc
#NVCCFLAGS =  -arch=sm_60 -O -g
LDFLAGS   =  -O3
CFLAGS    =  -O3  
#CFLAGS    =  -O3  -DDEBUG1

OBJECTS	  = it_mult_vec.o it_mult_vec_test.o minunit.o

TARGET		= it_mult_vec_test

all:  $(TARGET)

it_mult_vec_test: $(OBJECTS) minunit.h Makefile
	$(NVCC) -o $@ $(OBJECTS) $(LDFLAGS)

#Make sure you load these modules before compiling with this makefile
#	module purge
#	module load slurm
#	module load gpu/0.15.4 gcc/7.2.0 cuda/11.0.2
status:
	squeue -u `whoami`

run-it_mult_vec_test: it_mult_vec_test
	sbatch -v it_mult_vec_test.sh

use1:
	srun --pty --nodes=1 --ntasks-per-node=1 --cpus-per-task=10  -p gpu-debug --gpus=1 -t 00:10:00 -A csb175 /bin/bash

%.o: %.cu it_mult_vec.h minunit.h Makefile
	$(NVCC) $(NVCCFLAGS) $(CFLAGS) -o $@ -c $<

.c.o:
	$(NVCC) $(CFLAGS) -c $<

clean:
	rm  *.o $(TARGET)

cleanlog:
	rm  *.out
