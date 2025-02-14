#CC      = icc
CC      = gcc
CFLAGS  =   -O -fopenmp 
LDFLAGS  =  -lm
#CFLAGS  =  -O -DDEBUG1 -g

OBJECTS1= itmv_mult_omp.o itmv_mult_test_omp.o  minunit.o 

TARGET= itmv_mult_test_omp

all:  $(TARGET)


itmv_mult_test_omp: $(OBJECTS1) itmv_mult_omp.h minunit.h 
	$(CC) -o $@ $(OBJECTS1) $(LDFLAGS) $(CFLAGS)

status:
	squeue -u `whoami`

run-itmv_mult_test_omp:
	sbatch -v run-itmv_mult_test_omp.sh
	
.c.o: 
	$(CC)  $(CFLAGS) -c $<

clean:
	rm  *.o $(TARGET)

cleanlog:
	rm  job*.out
