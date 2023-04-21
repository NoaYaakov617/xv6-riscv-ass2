#include "uthread.h"
#include "user.h"
struct uthread uthreads_table [MAX_UTHREADS];
struct uthread *current_thread;
int on = 0;


int uthread_create(void (*start_func)(), enum sched_priority priority){
    struct uthread *t;
    for (t = uthreads_table; t < &uthreads_table[MAX_UTHREADS]; t++)
    {
        if(t->state == FREE){
            // Set up the new thread's stack
            if(t->ustack == 0){
                return -1;
            }
            t->context.sp = (uint64)t->ustack + STACK_SIZE;
            t->context.ra = (uint64)start_func;
            t->priority = priority;
            t->state = RUNNABLE;
            return 0;
        }
    }
    return -1;
    }
    

void uthread_yield(){
    struct uthread *curr_thread;
    struct uthread *max_thread = uthreads_table;
    int max_priority_counter = 0;
    enum sched_priority max_priority = max_thread->priority;
    for (curr_thread = uthreads_table; curr_thread < &uthreads_table[MAX_UTHREADS]; curr_thread++)
    {
        if(curr_thread->state == RUNNABLE){
            if(curr_thread->priority > max_priority){
                max_priority = curr_thread->priority;
                max_thread = curr_thread;
                max_priority_counter = 1;

            }
            else if (curr_thread->priority == max_priority)
            {
                max_priority_counter++;
            }    
            
        }
    }
    if (max_priority_counter > 1 && max_thread == current_thread)
    {
        for (curr_thread = uthreads_table; curr_thread < &uthreads_table[MAX_UTHREADS]; curr_thread++)
        {
        if(curr_thread->state == RUNNABLE && curr_thread->priority == max_priority && curr_thread != current_thread){
                max_thread = curr_thread;
            }


        }
    
    current_thread->state = RUNNABLE;
    max_thread->state = RUNNING;
    uswtch(&current_thread->context,&max_thread->context);


    }
}

void uthread_exit(){
    struct uthread *curr_thread;
    int num_of_runable_threads = 0;
    for (curr_thread = uthreads_table; curr_thread < &uthreads_table[MAX_UTHREADS]; curr_thread++)
    {
        if(curr_thread->state == RUNNABLE ){
            num_of_runable_threads++;
            break;
        }  
    }
    if(num_of_runable_threads == 0){
        current_thread->state = FREE;
        exit(0);
    }

  
    struct uthread *max_thread = uthreads_table;
    enum sched_priority max_priority = max_thread->priority;
    for (curr_thread = uthreads_table; curr_thread < &uthreads_table[MAX_UTHREADS]; curr_thread++)
    {
        if(curr_thread->state == RUNNABLE && curr_thread->priority > max_priority && curr_thread != current_thread){
                max_priority = curr_thread->priority;
                max_thread = curr_thread;
            } 
    }
    current_thread->state = FREE;
    max_thread->state = RUNNING;
    uswtch(&current_thread->context,&max_thread->context);

    }



int uthread_start_all(){
   
    if(on == 0){
        on = 1;
 
    struct uthread *curr_thread;
    struct uthread *max_thread = uthreads_table;
    enum sched_priority max_priority = max_thread->priority;
    for (curr_thread = uthreads_table; curr_thread < &uthreads_table[MAX_UTHREADS]; curr_thread++)
    {
        if(curr_thread->state == RUNNABLE){
            if(curr_thread->priority > max_priority){
                max_priority = curr_thread->priority;
                max_thread = curr_thread;
            }
   
        }

    }

    current_thread = max_thread;
    current_thread->state = RUNNING;
    uswtch(&current_thread->context,&current_thread->context);
    return 0;

    }
    

    else{
        return -1;
    }


}
enum sched_priority uthread_set_priority(enum sched_priority priority){
    enum sched_priority prev_priority = current_thread->priority;
    current_thread->priority = priority;
    return prev_priority;
}

enum sched_priority uthread_get_priority(){
    return current_thread->priority;
}

struct uthread* uthread_self(){
    return current_thread;
}

