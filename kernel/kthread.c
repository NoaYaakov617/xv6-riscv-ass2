#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];
extern struct spinlock wait_lock;



void kthreadinit(struct proc *p)
{
  
  
  acquire(&wait_lock);
  
  acquire(&p->lock);
 
  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->tlock, "thread_lock");
  
    kt->tstate = TUNUSED;
    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
    
  }
 
  release(&p->lock);
  release(&wait_lock);
  
  
}

struct kthread *mykthread()
{
  
  struct proc *p = myproc();
  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    if(kt->tstate == RUNNING){
      return kt;
    }
  }
  return 0;

}
int
  alloctpid(struct proc *p){
    int tpid;
    acquire(&p->counter_lock);
    tpid = p->tcounter ;
    p->tcounter++;
    release(&p->counter_lock);
    return tpid;
  }


struct kthread*
allockthread(struct proc *p){
 
  struct kthread *kt ;
  
 

  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    acquire(&kt->tlock);
    if(kt->tstate == TUNUSED){
      goto found;
    }

    else{
      release(&kt->tlock);
    }

  }
   
  return 0;  
  found:
  
  kt->tpid = alloctpid(p);
  
  kt->tstate = TUSED;
  
//feching the trapframe using the required method. the base trapframe of the process should be set before this fetch
  kt->trapframe = get_kthread_trapframe(p, kt);

  memset(&kt->context, 0, sizeof(kt->context));
  kt->context.ra = (uint64)forkret;
  kt->context.sp = kt->kstack + PGSIZE;


  return kt;


  }

void
 freekthread(struct kthread *kt){

  kt->tpid = 0;
  kt->tchan = 0;
  kt->tkilled = 0;
  kt->txstate = 0;
  kt->pcb = 0;
  kt->kstack = 0; // do we need to free the stack?
  kt->tstate = TUNUSED;
  // what about the trapframe and the context?

 }

struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}



// MultiThreads
int kthread_create( void *(*start_func)(), void *stack, uint stack_size){

}

int kthread_id(void){
  return mykthread()->tpid;
}

int kthread_kill(int ktid){

  struct proc *p = myproc();
  acquire(&p->lock);
  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    
    if(kt->tpid == ktid){
      acquire(&kt->tlock);
      kt->tkilled = 1;
      if(kt->tstate == SLEEPING){
        kt->tstate = RUNNABLE;
      }
      release(&kt->tlock);
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
    return -1;
  }

  



}
void kthread_exit(int status){
  struct kthread *kt = mykthread();
  acquire(&kt->tlock);
  kt->txstate = status;
  kt->tstate = TZOMBIE;
  release(&kt->tlock);
}

int kthread_join(int ktid, int *status){
  struct proc *p = myproc();
  
  acquire(&wait_lock);
  for(;;){
  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    if(kt->tpid == ktid){
      acquire(&kt->tlock);
      if(kt->tstate == TZOMBIE){
        //if(kt->txstate != NULL){ 
          if(status != 0 && copyout(p->pagetable, status, (char *)&kt->txstate,
                                  sizeof(kt->txstate)) < 0) {
            release(&kt->tlock);
            release(&wait_lock);
            return -1;
            }
          ///   freeproc(pp); ?????/
          release(&kt->tlock);
          release(&wait_lock);
          return 0;
        }
        release(&kt->tlock);
    }
  
  }
  }
             
}

      
 


