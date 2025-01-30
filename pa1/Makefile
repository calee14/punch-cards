#if your account does not have mpicc, type "module reset" and "module load gcc openmpi"
# Jan 29, 2025 note: Use "module load gcc/10.2.0 openmpi/4.1.1"
#CC      = pgcc
#CC      = icc
CC      = gcc

MPICC      = mpicc
CFLAGS  =  -O
LDFLAGS  =  -O
#CFLAGS  =  -O -DDEBUG1


OBJECTS1=treesum_mpi.o treesum_test_mpi.o minunit.o

OBJECTS2=mv_mult_mpi.o mv_mult_test_mpi.o minunit.o

OBJECTS3=itmv_mult_mpi.o itmv_mult_test_mpi.o test_main_mpi.o minunit.o

TARGET= treesum_test_mpi mv_mult_test_mpi itmv_mult_test_mpi 

all:  $(TARGET)


treesum_test_mpi: $(OBJECTS1) minunit.h
	$(MPICC) -o $@ $(OBJECTS1) $(LDFLAGS)

mv_mult_test_mpi: $(OBJECTS2) minunit.h
	$(MPICC) -o $@ $(OBJECTS2) $(LDFLAGS)

itmv_mult_test_mpi: $(OBJECTS3) minunit.h
	$(MPICC) -o $@ $(OBJECTS3) $(LDFLAGS)

status:
	squeue -u `whoami`

run-treesum_test_mpi:
	sbatch -v run-treesum_test_mpi.sh

run-mv_mult_test_mpi:
	sbatch -v run-mv_mult_test_mpi.sh

run-itmv_mult_test_mpi:
	sbatch -v run-itmv_mult_test_mpi.sh

.c.o: 
	$(MPICC)  $(CFLAGS) -c $<

clean:
	rm  *.o $(TARGET)

cleanlog:
	rm  job*.out
