
// queue.c file
extern PROC* freeList;
extern PROC* readyQueue;
// WRITE YOUR OWN functions:
int tenqueue(TQ **tq, TQ *p)
{
    //printf("into QQQQQ");
    int sum = 0;
    TQ *t,*last = 0;
    if((*tq)==0)
    {
        //printf("\ninitial time queue\n");
        (*tq) = p;
        return;
    }

    if(p->expire < (*tq)->expire)
    {
        t = *tq;
        p->next = t;
        (*tq) = p;
        while(t)
        {
            (t->expire) = (t->expire) - (*tq)->expire;
            t = t->next;
        }

    }else if(p->expire >= (*tq)->expire)
    {
        t = (*tq);
        while((sum + (t->expire)) <= p->expire)
        {
            sum = sum + (t->expire);
            last = t;
            t = t->next;
            if(t == 0)
            {
                p->expire = (p->expire)-sum;
                last->next = p;
                p->next = 0;
                return;
            }
               
        }

        p->expire = (p->expire)-sum;
        p->next = last->next;
        last->next = p;
        t = p;
        while(p)
        {
            p = p->next;
            if(p != 0)
            {
                p->expire = (p->expire)-(t->expire);
            }
        }
        return;
    }

}

PROC* tdequeue(TQ **tq)
{
    TQ *head;
    TQ *k = (*tq)->next;
    head = *tq;
    *tq = k;
    return head;
}

int enqueue(PROC** queue, PROC* p)
{
   // kprintf("\n****p %d %dinto enqueue*****\n", p->pid, p->priority);
    int SR = int_off(); // IRQ interrupts off, return CPSR
                        // enter p into *queue by priority; PROCs with the same priority by FIFO;

    PROC* head = *queue;
    if ((*queue) == 0) {
        *queue = p;
        (*queue)->next = 0;

    } else {
        while (head != 0) {
    
            if (p->priority > (*queue)->priority) {
                p->next = (*queue);
                *queue = p;
                //kprintf("*queue = p;%d \n",(*queue)->pid);
                //printList("111readyQueue", readyQueue);
                break;
            } else if (head->next == 0) {
                //p->next = 0;
                head->next = p;

                break;
            } else if (head->next->priority < p->priority) {
                p->next = head->next;
                head->next = p;
         
                break;
            }
            head = head->next;
        }
    }

    int_on(SR); //  restore CPSR
}

PROC* dequeue(PROC** queue)
{

    int SR = int_off(); // IRQ interrupts off, return CPSR
                        // remove the FIsSRT element from *queue;
    PROC* p = *queue;

    PROC* k = (*queue)->next;
    *queue = k;
    p->next = 0;
    int_on(SR); //  restore CPSR

    return p;
}

int printList(char* name, PROC* p)
{
    kprintf("%s = ", name);
    while (p) {
        kprintf("[%d%d]->", p->pid, p->priority);
        p = p->next;
    }
    kprintf("NULL\n");
}
int printChildlist(char* name, PROC* p)
{
    kprintf("%s = ", name);
    while (p) {
        kprintf("[%d]->", p->pid);
        p = p->sibling;
    }
    kprintf("NULL\n");
}
int printTQ(char* name, TQ* tq)
{
    kprintf("%s = ", name);
    while (tq) {
        kprintf("[expire time %d]->", tq->expire);
        tq = tq->next;
    }
    kprintf("NULL\n");
}

