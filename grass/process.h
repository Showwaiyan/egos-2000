#pragma once

#include "egos.h"
#include "syscall.h"
#include <time.h>

enum proc_status {
  PROC_UNUSED,
  PROC_LOADING,
  PROC_READY,
  PROC_RUNNING,
  PROC_RUNNABLE,
  PROC_PENDING_SYSCALL,
  PROC_SLEEPING
};

struct process {
  int pid;
  struct syscall syscall;
  enum proc_status status;
  uint mepc, saved_registers[32];
  /* Student's code goes here (Preemptive Scheduler | System Call). */

  /* Add new fields for lifecycle statistics, MLFQ or process sleep. */
  time_t creation_t;
  time_t first_t_scheduled;
  time_t last_t_on_cpu;
  time_t accumulated_cpu_t;
  time_t turnaround_t;
  time_t respond_t;
  int interrupt_count;

  int mlfq_level;
  time_t mlfq_runtime;

  time_t sleep_start_time;
  uint sleep_usec;

  /* Student's code ends here. */
};
#define MAX_NPROCESS 16

ulonglong mtime_get();

int proc_alloc();
void proc_free(int);
void proc_set_ready(int);
void proc_set_running(int);
void proc_set_runnable(int);
void proc_set_pending(int);

void mlfq_reset_level();
void mlfq_update_level(struct process *p, ulonglong runtime);
void proc_sleep(int pid, uint usec);
void proc_coresinfo();

extern uint core_to_proc_idx[NCORES];
