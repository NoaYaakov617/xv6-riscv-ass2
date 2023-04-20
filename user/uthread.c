#include "uthread.h"
struct uthread uthreads_table [MAX_UTHREADS];
struct uthread *current_thread;


int uthread_create(void (*start_func)(), enum sched_priority priority){
    struct uthread *t;
    for (t = uthreads_table; t < &uthreads_table[MAX_UTHREADS]; t++)
    {
        if(t->state == FREE){
            // Set up the new thread's stack
            if(t->ustack == 0){
                return -1;
            }
            t->context.sp = (uint)t->ustack + STACK_SIZE;
            t->context.ra = start_func;
            t->priority = priority;
            t->state = RUNNABLE;
            return 0;
        }
    }
    return -1;
    }
    

void uthread_yield(){
    struct uthread *curr_thread;
    struct uthread *max_thread = 0;
    enum sched_priority max_priority = LOW;
    for (curr_thread = uthreads_table; curr_thread < &uthreads_table[MAX_UTHREADS]; curr_thread++)
    {
        if(curr_thread->state == RUNNABLE && curr_thread->priority > max_priority && curr_thread != uthread_self()){
                max_priority = curr_thread->priority;
                max_thread = curr_thread;
            } 
    }
    if(max_thread == 0){
        max_thread = uthread_self();
    }
    uswtch(&uthread_self()->context,&max_thread->context);
    

    }

void uthread_exit(){

}

int uthread_start_all(){

}
enum sched_priority uthread_set_priority(enum sched_priority priority){

}
enum sched_priority uthread_get_priority(){

}

struct uthread* uthread_self(){

}

