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

#include "type.h"
#include "vid.c"
#include "string.c"
#include "timer.c"
#include "queue.c"
#include "kernel.c"
#include "kbd.c"
#include "exceptions.c"
//#include "kernel.c"


void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    if (vicstatus & (1<<4)){   // timer0,1=bit4
         timer_handler(0);
    }

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}
char mybuffer[8];
int head=0,tail = 0;
PROC *blockQueue = 0;
SEMAPHORE empty;
SEMAPHORE full;
SEMAPHORE mutex;


int body(int pid, int ppid,int priority, int func);
int producer_task();
int consumer_task();
int producer_task()
{
    while(1)
    {
    char s[128];
    printf("producer_task is running, please input string:\n");
    //kgets(s);
    while(1)
    {
        char item;
        item = kgetc();
        //kputc(item);
        P(&empty);
        P(&mutex);
        kputc(item);
        mybuffer[head++] = item;
        head %= 8;
        V(&mutex);
        V(&full);

    }

    tswitch();
    }
}

int consumer_task()
{
    while(1)
    {
    printf("consumer_task is running\n");
    
    //kgetc();

    while(1)
    {
        char item;
        P(&full);
        P(&mutex);
        item = mybuffer[tail++];
        tail %= 8;
        kputc(item);
        V(&mutex);
        V(&empty);

    }


    tswitch();
    }
}
 

//char buffer[8];

int main()
{ 
   empty.value = 8;
   empty.queue = blockQueue;
   full.value = 0;
   full.queue = blockQueue;
   mutex.value = 1;
   mutex.queue = blockQueue;
   
   int i; 
   char line[128]; 
   u8 kbdstatus, key, scode;

   color = WHITE;
   row = col = 0; 

   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4
   VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5

   fbuf_init();
   timer_init();
   timer_start(0);
   kbd_init();
  
   /* enable KBD IRQ */
   VIC_INTENABLE |= 1<<31;  // SIC to VIC's IRQ31
   SIC_ENSET |= 1<<3;       // KBD int=3 on SIC

   kprintf("Welcome to WANIX in Arm\n");

   init();
   //kfork((int)body,1);
   kfork((int)producer_task, 1);
   kfork((int)consumer_task, 1);
   //tswitch();
   //tswitch();
   printList("readyQueue", readyQueue);
      
   while(1){
     if (readyQueue)
        tswitch();
   }
   
   
}
