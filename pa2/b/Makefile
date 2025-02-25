#CC      = icc
CC      = gcc
CFLAGS  = -O2
LDFLAGS = -lpthread -lm
#CFLAGS  =  -O -DDEBUG1 -g

OBJECTS2 = itmv_mult_pth.o itmv_mult_test_pth.o minunit.o 
OBJECTS3 = cs140barrier.o cs140barrier_test.o minunit.o

TARGET = itmv_mult_test_pth cs140barrier_test

all: $(TARGET)

pi: $(OBJECTS0)
	$(CC) -o $@ $(OBJECTS0)


itmv_mult_test_pth: $(OBJECTS2) itmv_mult_pth.h minunit.h
	$(CC) -o $@ $(OBJECTS2) $(LDFLAGS) $(CFLAGS)

cs140barrier_test: $(OBJECTS3)
	$(CC) -o $@ $(OBJECTS3) $(LDFLAGS) $(CFLAGS)

status:
	squeue -u `whoami`


localrun-itmv:
	./itmv_mult_test_pth 4

localrun-cs140barrier:
	./cs140barrier_test


run-itmv_mult_test_pth:
	sbatch -v run-itmv_mult_test_pth.sh

run-cs140barrier_test:
	sbatch -v run-cs140barrier_test_pth.sh

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o $(TARGET)

cleanlog:
	rm job*.out
