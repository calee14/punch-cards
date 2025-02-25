/*
 * File: cs140barrier_test.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cs140barrier.h"
#include "minunit.h"

/* The total round cs140barrier is used. */
#define TOTAL_ROUND 100

/* Information passed into each thread. */
typedef struct {
  /* wait_count: count the output for each thread from cs140barrier_wait. */
  int *wait_count;
  /* bstate: a pointer to a cs140barrier. */
  cs140barrier *bstate;
} ThreadArgs;

/*-------------------------------------------------------------------
 * This function is executed by each thread.
 * If any error in cs140barrier, return that error message.
 * If successful, return NULL.
 */
void *threadFunc(void *args) {
  ThreadArgs *thread_args = args;
  cs140barrier *bstate = thread_args->bstate;
  int *wait_count = thread_args->wait_count;
  int i, is_odd_round, expected_is_odd_round,
      wait_info /* output from cs140barrier_wait */;
  char *err;

  for (i = 0; i < TOTAL_ROUND; i++) {
    is_odd_round = bstate->odd_round == True;
    expected_is_odd_round = i % 2;
    err =
        mu_check_assert("Something wrong. Threads are not in the same round.\n",
                        is_odd_round == expected_is_odd_round);

    if (err) {
      return (void *)err;
    }

    /* info should be either 0 or 1.
     * In each round, only one (arbitrary) thread gets 1.
     */
    wait_info = cs140barrier_wait(bstate);
    err = mu_check_assert(
        "Something wrong. cs140barrier_wait should output either 1 or 0.\n",
        wait_info == 1 || wait_info == 0);

    if (err) {
      return (void *)err;
    }

    *wait_count += wait_info;
    usleep(random() % 100);
  }

  return NULL;
}

/*-------------------------------------------------------------------
 * Test barrier initialization.
 * If failed, return a message string showing the failed point.
 * If successful, return NULL.
 */
char *barrier_init_test(void) {
  cs140barrier *bstate = malloc(sizeof(cs140barrier));
  printf("Test function cs140barrier_init()\n");
  char *err = mu_check_assert("Failed to initialize a new barrier.\n",
                              cs140barrier_init(bstate, 1) == 0);

  if (!err) {
    err = mu_check_assert(
        "cs140barrier.odd_round should be initialized to be False.\n",
        bstate->odd_round == False);
  }

  free(bstate);

  return err;
}

/*-------------------------------------------------------------------
 * Test barrier used for <nthread> threads.
 * If failed, return a message string showing the failed point.
 * If successful, return NULL.
 */
char *barrier_thread_test(int nthread) {
  int i, sum_wait_count = 0;
  char *err;
  void *thread_err;
  int *all_wait_count = malloc(nthread * sizeof(int));
  ThreadArgs *thread_args = malloc(nthread * sizeof(ThreadArgs));
  pthread_t *tha = malloc(nthread * sizeof(pthread_t));

  printf("Test function cs140barrier_init/wait with %d threads\n", nthread);

  cs140barrier *bstate = malloc(sizeof(cs140barrier));
  err = mu_check_assert("Failed to initialize a new barrier.\n",
                        cs140barrier_init(bstate, nthread) == 0);

  if (err) return err;

  for (i = 0; i < nthread; i++) {
    /* Initialize wait_count for each thread to be 0. */
    all_wait_count[i] = 0;
    thread_args[i].bstate = bstate;
    thread_args[i].wait_count = all_wait_count + i;
    err = mu_check_assert("Failed to initialize a new thread.\n",
                          pthread_create(&tha[i], NULL, threadFunc,
                                         (void *)(thread_args + i)) == 0);

    if (err) return err;
  }

  for (i = 0; i < nthread; i++) {
    err = mu_check_assert("Failed to join a thread.\n",
                          pthread_join(tha[i], &thread_err) == 0);

    if (err) return err;

    if (thread_err) return (char *)thread_err;

    sum_wait_count += all_wait_count[i];
  }

  err = mu_check_assert(
      "In cs140barrier_wait, one and only one thread outputs 1.\n",
      sum_wait_count == TOTAL_ROUND);

  if (err) return err;

  free(tha);
  free(all_wait_count);
  free(bstate);
  free(thread_args);

  return NULL;
}

/*-------------------------------------------------------------------
 * Run barrier_thread_test with 1 thread.
 */
char *barrier_one_thread_test() { return barrier_thread_test(1); }

/*-------------------------------------------------------------------
 * Run barrier_thread_test with a number of threads.
 */
char *barrier_multi_thread_test() { return barrier_thread_test(4); }
char *barrier_multi_thread_test1() { return barrier_thread_test(11); }
char *barrier_multi_thread_test2() { return barrier_thread_test(16); }

/*-------------------------------------------------------------------
 * Run all tests.  Ignore returned messages.
 */
void run_all_tests(void) {
  /* Call all tests.  You can add more tests*/
  mu_run_test(barrier_init_test);
  mu_run_test(barrier_one_thread_test);
  mu_run_test(barrier_multi_thread_test);
  mu_run_test(barrier_multi_thread_test1);
  mu_run_test(barrier_multi_thread_test2);
}

/*-------------------------------------------------------------------
 * The main entrance to run all tests.
 * If failed, return a message string showing the first failed point.
 * Print the test stats.
 */
int main(int argc, char *argv[]) {
  run_all_tests();

  mu_print_test_summary("Summary:");
  return 0;
}
