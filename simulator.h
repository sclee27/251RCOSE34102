#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "process.h"
#include "queue.h"

#define FCFS 1    // Priority check을 check함수로 + all priority set to 1
#define np_SJF 2  // CPU time check을 check함수로 
#define np_Pri 3  // Priority check을 check함수로
#define RR 4      // Priority check을 check함수로 + all priority set to 1 + Timer()
#define p_SJF 5   // CPU time check을 check함수로 + check When ready que pushed
#define p_Pri 6   // Priority check을 check함수로 + check When ready que pushed

int pre_stop_condition(Process * running_p, Process * newly_entered_p, 
                            int check_for_ready_q(Process *p));


int nonpre_stop_condition(Process * running_p, Process * newly_entered_p, 
                            int check_for_ready_q(Process *p));

int base_stop_condition(Process * running_p, int current_time_quantum);

int schedule(int policy, int (** check_for_ready_q)(Process *), int (** pnp_stop_condition) (Process *, Process *, int (*) (Process *)), int * time_quantum);

int simulator(HEADER * job_queue, HEADER ** ready_que, HEADER ** wait_que,  
                int check_for_ready_q(Process * p), 
                int preemptive_stop_condition(Process * running_p, Process * newly_entered_p, int check_for_ready_q(Process *p)), 
                int time_quantum);

#endif