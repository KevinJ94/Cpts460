/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf
 
typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;

  int    priority;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    kstack[SSIZE];
}PROC;
***************************/
#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue;
int procsize = sizeof(PROC);
int body(int pid, int ppid, int func, int priority);
//int enqueue(PROC** queue, PROC* p);
//PROC* dequeue(PROC** queue);
int ksleep(int event)
{
    int SR = int_off();
    running->event = event;
    running->status = SLEEP;
    printf("[%d]sleep on event %x \n",running->pid,running->event);
    tswitch();
    int_on(SR);
}

int kwakeup(int event)
{
    int SR = int_off();
    int i;
    PROC *p;
    for (i=1; i<NPROC; i++){
        p = &proc[i];
        if(p->status == SLEEP && p->event == event)
        {
            p->event = 0;
            p->status = READY;
            enqueue(&readyQueue, p);
        }
    }
    int_on(SR);
}
int kwait(int *status)
{
    
    PROC *p,*t; int i,isSleep;
    if(running->child == 0)
    {
        return -1;
    }
    for (i=1; i<NPROC; i++){
        p = &proc[i];
        if (p->status == ZOMBIE){
            isSleep = 1;
            *status = p->exitCode;
            t = p->parent;
            if(t->child == p)
            {
                t->child = p->sibling;
            }
            else
            {
                t = t->child;
                while(t->sibling != p)
                {
                t = t->sibling;
                }
                t->sibling = p->sibling;
                printf("killed [%d] exitCode: %d",p->pid,p->exitCode);
            }
        p->status = FREE;
        enqueue(&freeList, p);
        return(p->pid);
        }
        
    }
    if(isSleep)
    {ksleep(running);}
    
    
}
int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;

  printf("create P0 as initial running process\n");
  p = dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0; p->parent = p;  // P0's parent is itself
  running = p;
  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}
/*
void kexit()
{
  printf("proc %d kexit\n", running->pid);
  running->status = FREE;
  running->priority = 0;
  enqueue(&freeList, running);   // putproc(running);
  tswitch();
}
*/
int kexit()
{
    int i,iswakeupP1,exitValue = 0;
    PROC *p;
    char s[128];
    printf("please input exitValue:");
    kgets(s);
    //printf("testxxx:%s",s);
    exitValue = geti(s);
    if (running->pid==1){ 
        printf("P1 cannot die\n");
        return -1;
        }
    /*
    for (i = 1; i < NPROC; i++){
        p = &proc[i];
        if (p->status != FREE && p->ppid == running->pid){
            p->ppid = 1;
            p->parent = &proc[1];
            iswakeupP1++;
        }
    }
    */
    if(running->child != 0)
    {
        PROC *head,*t;
        head = running->child;
        t = &proc[1];
        if(t->child != 0)
        {
            t = t->child;
            while(t->sibling)
            {t = t->sibling;}
            t->sibling = head;
        }
        else
        {
            t->child = head;
        }
        while(head)
        {
            head->ppid = 1;
            head->parent = &proc[1];
            iswakeupP1++;
            head = head->sibling;
        }
    }
    running->exitCode = exitValue;
    running->status = ZOMBIE;
    kwakeup(running->parent); // parent sleeps on its PROC address
    if (iswakeupP1)
        kwakeup(&proc[1]);
    kprintf("\ntest exitValue[%d]\n",exitValue);
    tswitch();
}

PROC *kfork(int func, int priority)
{
  int i;
  PROC *head = 0;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;
  if(running->child == 0)
  {
      running->child = p;
      p->sibling = 0;
  }
  else
  {
      head = running->child;
      while(head->sibling)
      {
          head = head->sibling;
      }
      head->sibling = p;
  }

  
  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-14] = p->pid;
  p->kstack[SSIZE-13] = p->ppid;
  p->kstack[SSIZE-12] = (int)func;
  p->kstack[SSIZE-11] = p->priority;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);
  printf("%d kforked a child %d\n", running->pid, p->pid);
  kprintf("child[%d]",running->parent->child->pid);
  printChildlist("childlist:",running->child);

  printList("readyQueue", readyQueue);
  return p;
}

int scheduler()
{
  //  kprintf("===========start==========\n");
  kprintf("proc %d in schedule ", running->pid);
  if (running->status == READY){
      enqueue(&readyQueue, running);
      //kprintf("in schediler55555\n");
  }
  running = dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
 // kprintf("===========end==========\n");
}  


int body(int pid, int ppid, int func, int priority)
{
  char c; char line[64];
  //int pid;
  int status;
  kprintf("pid %d \n",  pid);
  kprintf("ppid %d \n",  ppid);
  kprintf("priority %d \n",priority);
  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    pid = running->pid;
    if (pid==0) color=BLUE;
    if (pid==1) color=WHITE;
    if (pid==2) color=GREEN;
    if (pid==3) color=CYAN;
    if (pid==4) color=YELLOW;
    if (pid==5) color=WHITE;
    if (pid==6) color=GREEN;   
    if (pid==7) color=WHITE;
    if (pid==8) color=CYAN;
    
    printList("readyQueue", readyQueue);
   // kprintf("%d %d",i,j);
    kprintf("proc %d running, parent = %d  ", running->pid, running->ppid);
    kprintf("child[%d]",running->parent->child->pid);
    printChildlist("childlist:",running->child);
    kprintf("input a char [s|f|q|w] : ");
    c = kgetc(); 
    printf("%c\n", c);

    switch(c){
      case 's': tswitch();           break;
      case 'f': kfork((int)body, 1); break;
      case 'q': kexit();             break;
      case 'w': pid = kwait(&status);  if(pid == -1){printf("erroe no child");}break;
    }
  }
}

