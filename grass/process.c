/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: helper functions for process management
 */

#include "process.h"

#define MLFQ_NLEVELS 5
#define MLFQ_RESET_PERIOD 10000000             /* 10 seconds */
#define MLFQ_LEVEL_RUNTIME(x) (x + 1) * 100000 /* e.g., 100ms for level 0 */
extern struct process proc_set[MAX_NPROCESS + 1];

static void proc_set_status(int pid, enum proc_status status) {
  for (uint i = 0; i < MAX_NPROCESS; i++)
    if (proc_set[i].pid == pid)
      proc_set[i].status = status;
}

void proc_set_ready(int pid) { proc_set_status(pid, PROC_READY); }
void proc_set_running(int pid) { proc_set_status(pid, PROC_RUNNING); }
void proc_set_runnable(int pid) { proc_set_status(pid, PROC_RUNNABLE); }
void proc_set_pending(int pid) { proc_set_status(pid, PROC_PENDING_SYSCALL); }

int proc_alloc() {
  static uint curr_pid = 0;
  for (uint i = 1; i <= MAX_NPROCESS; i++)
    if (proc_set[i].status == PROC_UNUSED) {
      proc_set[i].pid = ++curr_pid;
      proc_set[i].status = PROC_LOADING;
      /* Student's code goes here (Preemptive Scheduler | System Call). */

      /* Initialization of lifecycle statistics, MLFQ or process sleep. */
      proc_set[i].creation_t = mtime_get();
      proc_set[i].first_t_scheduled = 0;
      proc_set[i].last_t_on_cpu = 0;
      proc_set[i].accumulated_cpu_t = 0;
      proc_set[i].turnaround_t = 0;
      proc_set[i].respond_t = 0;
      proc_set[i].interrupt_count = 0;
      proc_set[i].mlfq_level = 0;
      proc_set[i].mlfq_runtime = 0;

      /* Student's code ends here. */
      return curr_pid;
    }

  FATAL("proc_alloc: reach the limit of %d processes", MAX_NPROCESS);
}

void proc_free(int pid) {
  /* Student's code goes here (Preemptive Scheduler). */

  /* Print the lifecycle statistics of the terminated process or processes. */
  /* Helper: finalize stats + print, then free. */
  auto void free_one(int i) {
    struct process *p = &proc_set[i];

    /* Take termination timestamp. */
    time_t term = mtime_get();

    /* Close out last CPU slice if it is currently running. */
    if (p->status == PROC_RUNNING && p->last_t_on_cpu != 0) {
      p->accumulated_cpu_t += term - p->last_t_on_cpu;
      p->last_t_on_cpu = term;
    }

    /* Compute lifecycle metrics (raw mtime units). */
    time_t turnaround = (p->creation_t != 0) ? (term - p->creation_t) : 0;
    time_t response = (p->creation_t != 0 && p->first_t_scheduled != 0)
                          ? (p->first_t_scheduled - p->creation_t)
                          : 0;
    time_t cpu_time = p->accumulated_cpu_t;

    /* Convert to ms: book notes QEMU uses 10^-7 seconds per tick => /10000 for
     * ms. */
    ulonglong turnaround_ms = (ulonglong)(turnaround / 10000);
    ulonglong response_ms = (ulonglong)(response / 10000);
    ulonglong cpu_ms = (ulonglong)(cpu_time / 10000);

    printf("[INFO] process %d terminated after %d timer interrupts, "
           "turnaround time: %d, response time: %d, CPU time: %d\n",
           p->pid, (ulonglong)p->interrupt_count, turnaround_ms, response_ms,
           cpu_ms);

    earth->mmu_free(p->pid);
    proc_set_status(p->pid, PROC_UNUSED);
    return;
  }

  int idx = -1;
  for (uint i = 0; i < MAX_NPROCESS; i++)
    if (proc_set[i].pid == pid) {
      idx = i;
      break;
    }

  if (pid != GPID_ALL) {
    if (idx >= 0 && idx < MAX_NPROCESS && proc_set[idx].status != PROC_UNUSED) {
      free_one(idx);
    }
    // earth->mmu_free(pid);
    // proc_set_status(pid, PROC_UNUSED);
  } else {
    /* Free all user processes. */
    for (uint i = 0; i < MAX_NPROCESS; i++)
      if (proc_set[i].pid >= GPID_USER_START &&
          proc_set[i].status != PROC_UNUSED) {
        free_one(i);
        // earth->mmu_free(proc_set[i].pid);
        // proc_set[i].status = PROC_UNUSED;
      }
  }
  /* Student's code ends here. */
}

void mlfq_update_level(struct process *p, ulonglong runtime) {
  /* Student's code goes here (Preemptive Scheduler). */

  /* Update the MLFQ-related fields in struct process* p after this
   * process has run on the CPU for another runtime microseconds. */

  p->mlfq_runtime += runtime;
  if (p->mlfq_runtime > MLFQ_LEVEL_RUNTIME(p->mlfq_level)) {
    p->mlfq_runtime -= MLFQ_LEVEL_RUNTIME(p->mlfq_level);
    if (p->mlfq_level < 4) p->mlfq_level++;
  }

  /* Student's code ends here. */
}

void mlfq_reset_level() {
  /* Student's code goes here (Preemptive Scheduler). */
  if (!earth->tty_input_empty()) {
    /* Reset the level of GPID_SHELL if there is pending keyboard input. */
    
    for (uint i = 0; i < MAX_NPROCESS; i++) 
      if (proc_set[i].pid == GPID_SHELL) {
        proc_set[i].mlfq_level = 0;
        proc_set[i].mlfq_runtime = 0;
      }
  }

  static ulonglong MLFQ_last_reset_time = 0;
  /* Reset the level of all processes every MLFQ_RESET_PERIOD microseconds. */
  time_t now = mtime_get();
  if (MLFQ_last_reset_time = 0) MLFQ_last_reset_time = now;
  else if (now - MLFQ_last_reset_time >= MLFQ_RESET_PERIOD) {
    for (uint i = 0; i< MAX_NPROCESS; i++) {
      if (proc_set[i].status != PROC_UNUSED) {
        proc_set[i].mlfq_level = 0;
        proc_set[i].mlfq_runtime = 0;
      }
    }
    MLFQ_last_reset_time = now;
  }

  /* Student's code ends here. */
}

void proc_sleep(int pid, uint usec) {
  /* Student's code goes here (System Call & Protection). */

  /* Update the sleep-related fields in the struct process for process pid. */

  /* Student's code ends here. */
}

void proc_coresinfo() {
  /* Student's code goes here (Multicore & Locks). */

  /* Print out the pid of the process running on each CPU core. */

  /* Student's code ends here. */
}
