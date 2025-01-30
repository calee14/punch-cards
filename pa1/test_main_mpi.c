/*
 * File:     test_main_mpi.c
 * Call test code
 */

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"

void testmain();

/*-------------------------------------------------------------------
 * The main entrance to run all tests.
 */
int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  testmain();

  MPI_Finalize();
  return 0;
}
