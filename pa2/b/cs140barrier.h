/*
 * File: cs140barrier.h
 *
 * Everything here should be untouched.
 *
 */

#ifndef _BARRIER_CS140
#define _BARRIER_CS140

#include <pthread.h>

typedef enum { False, True } boolean;

typedef struct {
  pthread_mutex_t barrier_mutex;
  pthread_cond_t barrier_cond;
  /* The number of threads using this barrier. */
  int total_nthread;
  /* The number of threads that have reached this round. */
  int arrive_nthread;
  /* Indicate whether the number of this barrier having been used is odd. */
  boolean odd_round;
} cs140barrier;

int cs140barrier_init(cs140barrier *bstate, int total_nthread);

int cs140barrier_wait(cs140barrier *bstate);

int cs140barrier_destroy(cs140barrier *bstate);

#endif
