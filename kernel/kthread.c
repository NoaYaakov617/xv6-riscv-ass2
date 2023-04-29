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
  
  acquire(&p->lock);

  initlock(&p->counter_lock, "thread_counter_lock");

  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->tlock, "thread_lock");
  
    kt->tstate = TUNUSED;

    kt->pcb = p;

    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
    
  }
 
  release(&p->lock);
  
  
}

struct kthread *mykthread()
{
  push_off();
  struct cpu *c = mycpu();
  struct kthread *kt = c->kthread;
  pop_off();
  return kt;

}
int
  alloctpid(struct proc *p){
    int tpid;
    acquire(&p->counter_lock);
    tpid = p->tcounter;
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
  kt->context.sp = kt->kstack +  PGSIZE; 

  return kt;


  }

void
 freekthread(struct kthread *kt){
  
 
  kt->tpid = 0;
  kt->trapframe = 0;
  kt->tchan = 0;
  kt->tkilled = 0;
  kt->txstate = 0;
  kt->tstate = TUNUSED;
  kt->pcb = 0;

 

 }

struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}


