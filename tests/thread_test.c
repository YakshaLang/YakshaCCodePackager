#define YK__THREAD_IMPLEMENTATION
#include "yk__thread.h"
#include <stdio.h>// for printf
int yk__thread_proc(void *user_data) {
  yk__thread_timer_t timer;
  yk__thread_timer_init(&timer);
  int count = 0;
  yk__thread_atomic_int_t *exit_flag = (yk__thread_atomic_int_t *) user_data;
  while (yk__thread_atomic_int_load(exit_flag) == 0) {
    printf("Thread... ");
    yk__thread_timer_wait(&timer, 1000000000);// sleep for a second
    ++count;
  }
  yk__thread_timer_term(&timer);
  printf("Done\n");
  return count;
}
int main(int argc, char **argv) {
  (void) argc, argv;
  yk__thread_atomic_int_t exit_flag;
  yk__thread_atomic_int_store(&exit_flag, 0);
  yk__thread_ptr_t yk__thread = yk__thread_create(yk__thread_proc, &exit_flag, "Example thread",
                                      YK__THREAD_STACK_SIZE_DEFAULT);
  yk__thread_timer_t timer;
  yk__thread_timer_init(&timer);
  for (int i = 0; i < 5; ++i) {
    printf("Main... ");
    yk__thread_timer_wait(&timer, 2000000000);// sleep for two seconds
  }
  yk__thread_timer_term(&timer);
  yk__thread_atomic_int_store(&exit_flag, 1);// signal yk__thread to exit
  int retval = yk__thread_join(yk__thread);
  printf("Count: %d\n", retval);
  yk__thread_destroy(yk__thread);
  return retval;
}