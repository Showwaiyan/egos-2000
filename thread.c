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

struct thread *current_thread;
int current_idx;
// int previous_idx;
// struct thread TCB[MAX_THREADS] = {0};
SLIST_HEAD(TCB, thread) TCB;

/* Student's code ends here. */

void thread_init() {
  /* Student's code goes here (Cooperative Threads). */

  // Initialize the thread with head
  SLIST_INIT(&TCB);

  // Initialize the main thread
  struct thread *main = malloc(sizeof(struct thread));

  // Assigne value to main thread
  main->id = 0;
  main->status = RUNNING;
  main->stack_base = NULL;
  main->sp = NULL;
  main->entry = NULL;
  main->arg = NULL;

  current_idx = 0;
  current_thread = main;

  SLIST_INSERT_HEAD(&TCB, main, next);

  /* Student's code ends here. */
}

void ctx_entry() {
  /* Student's code goes here (Cooperative Threads). */

  struct thread *t = current_thread;
  // Call entery function with entery args
  t->entry(t->arg);

  // Call exit function
  thread_exit();

  /* Student's code ends here. */
}

void thread_create(void (*entry)(void *arg), void *arg) {
  /* Student's code goes here (Cooperative Threads). */

  struct thread *new_thread = malloc(sizeof(struct thread));

  current_thread->status = RUNNABLE;

  new_thread->id = ++current_idx;
  new_thread->status = RUNNING;
  new_thread->entry = entry;
  new_thread->arg = arg;

  struct thread *old_thread = current_thread;
  current_thread = new_thread;



  // Allocate Stack size
  char *child_stack = malloc(STACK_SIZE);
  // Why we use char*, because we need to process in raw bytes, so 1 byte size
  // char is the best

  new_thread->stack_base = child_stack;
  new_thread->sp = child_stack +
          STACK_SIZE; // We add STACK_SIZE as mallco only give the lowest memory
  // Stack need to point to highest memory of its

  SLIST_INSERT_HEAD(&TCB, new_thread, next);

  // Call ctx_start
  ctx_start(&old_thread->sp, new_thread->sp);

  /* Student's code ends here. */
}

void thread_yield() {
  /* Student's code goes here (Cooperative Threads). */
  struct thread *old_thread = current_thread;
  struct thread *new_thread = NULL;


  struct thread *t;
  SLIST_FOREACH(t, &TCB, next) {
    if (t->status == RUNNABLE) {
      new_thread = t;
      break;
    }
  }
  
  old_thread->status = RUNNABLE;
  new_thread->status = RUNNING;

  current_thread = new_thread;

  ctx_switch(&old_thread->sp, new_thread->sp);

  /* Student's code ends here. */
}

void thread_exit() {
  /* Student's code goes here (Cooperative Threads). */
  struct thread *old_thread = current_thread;
  
  old_thread->status = TERMINATED;
  old_thread->entry = NULL;
  old_thread->arg = NULL;


  struct thread *new_thread;
  struct thread *t;
  SLIST_FOREACH(t, &TCB, next) {
    if (t->status == RUNNABLE) {
      new_thread = t;
      break;
    }
  }

  if (new_thread == NULL) _end();
  new_thread->status = RUNNING;
  current_thread = new_thread;

  ctx_switch(&old_thread->sp, new_thread->sp);

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
