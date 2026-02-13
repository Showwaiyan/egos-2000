/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: cooperative multithreading and synchronization
 */

#include "thread.h"
#include "print.c"
#include <stdlib.h>
#include <sys/queue.h>

/* Student's code goes here (Cooperative Threads). */
/* Define the TCB and helper functions (if needed) for multi-threading. */
int current_idx;
int previous_idx;
struct thread TCB[MAX_THREADS] = {0};

/* Student's code ends here. */

void thread_init() {
  /* Student's code goes here (Cooperative Threads). */

  // Initialize all thread as UNUSED
  for (int i = 0; i < MAX_THREADS; i++) {
    TCB[i].status = UNUSED;
  }

  // Initialize assign current as 0 and main thread as index 0
  current_idx = 0;
  TCB[current_idx].id = 0;
  TCB[current_idx].status = RUNNING;
  TCB[current_idx].stack_base = NULL;
  TCB[current_idx].sp = NULL;
  TCB[current_idx].entry = NULL;
  TCB[current_idx].arg = NULL;

  /* Student's code ends here. */
}

void ctx_entry() {
  /* Student's code goes here (Cooperative Threads). */

  struct thread *t = &TCB[current_idx];
  // Call entery function with entery args
  t->entry(t->arg);

  // Call exit function
  thread_exit();

  /* Student's code ends here. */
}

void thread_create(void (*entry)(void *arg), void *arg) {
  /* Student's code goes here (Cooperative Threads). */

  // Find unused tcb element to assign a new thread
  struct thread *t_old = &TCB[current_idx];
  struct thread *t_new = NULL;

  int i;
  for (i = 0; i < MAX_THREADS; i++) {
    if (TCB[i].status == UNUSED) {
      t_new = &TCB[i];
      break;
    }
  }

  t_old->status = RUNNABLE;

  t_new->id = i;
  current_idx = t_new->id;
  t_new->status = RUNNING;
  t_new->entry = entry;
  t_new->arg = arg;

  // Allocate Stack size
  char *child_stack = malloc(STACK_SIZE);
  // Why we use char*, because we need to process in raw bytes, so 1 byte size
  // char is the best

  t_new->stack_base = child_stack;
  t_new->sp = child_stack +
          STACK_SIZE; // We add STACK_SIZE as mallco only give the lowest memory
  // Stack need to point to highest memory of its

  // Call ctx_start
  ctx_start(&t_old->sp, t_new->sp);

  /* Student's code ends here. */
}

void thread_yield() {
  /* Student's code goes here (Cooperative Threads). */
  struct thread *t_old = &TCB[current_idx];
  struct thread *t_new = NULL;

  int i;
  for (i = 0;i< MAX_THREADS; i++) {
    if (TCB[i].status == RUNNABLE) {
      t_new = &TCB[i];
      break;
    }
  }
  
  t_old->status = RUNNABLE;
  t_new->status = RUNNING;

  current_idx = i;

  ctx_switch(&t_old->sp, t_new->sp);

  /* Student's code ends here. */
}

void thread_exit() {
  /* Student's code goes here (Cooperative Threads). */
  struct thread *t_old = &TCB[current_idx]; // old thread
  t_old->status = TERMINATED;
  t_old->entry = NULL;
  t_old->arg = NULL;

  int i;
  struct thread *t_new = NULL; // new thread
  for (i = 0; i < MAX_THREADS; i++) {
    if (TCB[i].status == RUNNABLE) {
      t_new = &TCB[i];
      break;
    }
  }

  if (t_new == NULL) _end();
  t_new->status = RUNNING;

  current_idx = i;

  ctx_switch(&t_old->sp, t_new->sp);

  // Need to clean previous stack
  // if (t->status == TERMINATED &&
  //     t->stack_base != NULL) {
  //   free(t->stack_base);
  //   t->stack_base = NULL;
  // }

  /* Student's code ends here. */
}

/* Student's code goes here (Cooperative Threads). */
/* Define helper functions (if needed) for conditional variables. */

/* Student's code ends here. */

void cv_init(struct cv *condition) {
  /* Student's code goes here (Cooperative Threads). */

  /* Student's code ends here. */
}

void cv_wait(struct cv *condition) {
  /* Student's code goes here (Cooperative Threads). */

  /* Student's code ends here. */
}

void cv_signal(struct cv *condition) {
  /* Student's code goes here (Cooperative Threads). */

  /* Student's code ends here. */
}

#define BUF_SIZE 3
void *buffer[BUF_SIZE];
int count = 0;
int head = 0, tail = 0;
struct cv nonempty, nonfull;

void produce(void *arg) {
  while (1) {
    while (count == BUF_SIZE)
      cv_wait(&nonfull);
    /* At this point, the buffer is not full. */

    /* Student's code goes here (Cooperative Threads). */
    /* Print out the producer ID with the arg pointer. */

    /* Student's code ends here. */
    buffer[tail] = arg;
    tail = (tail + 1) % BUF_SIZE;
    count += 1;
    cv_signal(&nonempty);
  }
}

void consume(void *arg) {
  while (1) {
    while (count == 0)
      cv_wait(&nonempty);
    /* At this point, the buffer is not empty. */

    /* Student's code goes here (Cooperative Threads). */
    /* Print out the consumer ID with the arg pointer. */

    /* Student's code ends here. */
    void *result = buffer[head];
    head = (head + 1) % BUF_SIZE;
    count -= 1;
    cv_signal(&nonfull);
  }
}

// void child(void *arg) { printf("%s is running.\n\r", arg); }
void child(void* arg) {
    for (int i = 0; i < 10; i++) {
        printf("%s is in for loop i=%d\n\r", arg, i);
        thread_yield();
    }
}

int main() {
  // thread_init();
  // thread_create(child, "Child thread");
  // printf("Main thread is running.\n\r");
  // thread_exit();

    thread_init();
    thread_create(child, "Child thread");
    for (int i = 0; i < 10; i++) {
        printf("Main thread is in for loop i=%d\n\r", i);
        thread_yield();
    }
    thread_exit();

  // int ID[500];
  // for (int i = 0; i < 500; i++)
  //   ID[i] = i;
  //
  // for (int i = 0; i < 500; i++)
  //   thread_create(consume, ID + i);
  //
  // for (int i = 0; i < 500; i++)
  //   thread_create(produce, ID + i);
  //
  // printf("main thread exits\n\r");
  // thread_exit();

  /* The control flow should NEVER get here. If the main thread is the last to
   * call thread_exit(), thread_exit() should terminate the program by calling
   * the _end() in thread.s.
   * If the main thread is not the last, thread_exit() will switch the context
   * to another thread. Later, when all the threads have called thread_exit(),
   * the last one calling it should then call _end() within thread_exit(). */
}
