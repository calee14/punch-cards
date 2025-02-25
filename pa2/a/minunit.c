/* File:     minunit.c
 *
 * Purpose:  a minimum unit test API
 *
 */

/* Simple C unit test API based on http://www.jera.com/techinfo/jtns/jtn002.html
  More extensive version is in https://github.com/siu/minunit
 */

#include "minunit.h"
#include <stdio.h>

int _mu_tests_run = 0;
int _mu_tests_failed = 0;
/*--------------------------------------------------------------------
  Function: mu_run_test with argument  test_fun
  Purpose:  Run a simple unit test specified by test_fun function pointer.
    During this run, variable _mu_tests_run increments by one.
    Variable _mu_tests_failed increments by one if this test fails.
  In arg: test_fun:  a pointer to a function which returns a string.
    This function has no argument and should use mu_assert to check an error and
    return a message.
  Return: If the test fails, the function should return a
    string describing the failing test. If the test passes,  returns NULL as 0
 */
char *mu_run_test(char *(*test_fun)()) {
  char *message = (*test_fun)();
  _mu_tests_run++;
  if (message) {
    _mu_tests_failed++;
  }
  return message;
}
void mu_print_test_summary(char *startmsg) {
  printf("%s Failed %d out of %d tests\n", startmsg, _mu_tests_failed,
         _mu_tests_run);
}

/*------------------------------
 *Get the elapsed time in seconds
 */
#include <sys/time.h>
double get_time() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec + t.tv_usec / 1000000.0;
}
