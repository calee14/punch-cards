/* File:     minunit.h
 *
 * Purpose:  Header file for a minimum unit test API
 *
 * Compile with error message printing:  gcc -DDEBUG
 *
 */

/* Simple C unit test API based on http://www.jera.com/techinfo/jtns/jtn002.html
  More extensive version is in https://github.com/siu/minunit
 */

/*--------------------------------------------------------------------
 * mu_assert is a macro that returns a string if the condition expression passed
 * to it is false. When using it in a sequence of assertions, a function returns
 * a message as soon as encoutering a false condition (normally it means error).
 *
 * The space of this message cannot be allocated on the stack.
 * It needs to be allocated with malloc or uses a global variable.
 */

#define mu_assert(message, condition) \
  do {                                \
    if (!(condition)) return message; \
  } while (0)

char* mu_run_test(char* (*test_fun)());
void mu_print_test_summary(char* startmsg);

double get_time();
