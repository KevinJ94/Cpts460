#define NPIPE 8
#define PSIZE 8


typedef struct pipe{
  char buf[PSIZE]; 	// circular data buffer
  int head, tail;	// circular buf index
  int data, room; 	// number of data & room in pipe
  int status; 		// FREE or BUSY
}PIPE;

PIPE pipe[NPIPE]; 	// global PIPE objects

int pipe_init()
{
  int i;
  for(i=0; i<NPIPE; i++){
      pipe[i].status = FREE;
  }
}

PIPE *create_pipe()
{
  PIPE *p;
  p = &pipe[0];
  p->head = 0; 
  p->tail = 0;
  p->data = 0;
  p->room = PSIZE;
  p->status = BUSY;
  return p;
}
//---------- Algorithm of pipe_read-------------
int read_pipe(PIPE *p, char *buf, int n)
{
  
  int r = 0;
  if (n<=0)
      return 0;
  //validate PIPE pointer p; 		// p->status must not be FREE
  if(p->status==FREE) 
      return -1;
  while(n){
      while(p->data){
          *buf++ = p->buf[p->tail++]; 	// read a byte to buf
           p->tail %= PSIZE;
           p->data--; p->room++; r++; n--;
           if (n==0) break;
      }
      kwakeup(&p->room);	 	// wakeup writers
      if (r) 				// if has read some data
          return r;			// pipe has no data
      ksleep(&p->data);			// sleep for data
  }
}

//---------- Algorithm of write_pipe -----------
int write_pipe(PIPE *p, char *buf, int n)
{
  int r = 0;
  if (n<=0){
      return 0;}
  //validate PIPE pointer p; 		// p->status must not be FREE
  while(n){
      while(p->room){
          p->buf[p->head++] = *buf++; 	// write a byte to pipe;
          p->head %= PSIZE;
          p->data++; p->room--; r++; n--;
          if (n==0) {
              break;}
       }
       kwakeup(&p->data);		// wakeup readers, if any.
       if (n==0) 
		{
			//int num = 0;
			//char line[128];
			//num = read_pipe(kpipe, line, 20);
			tswitch();
			return r;
		}
				// finished writing n bytes
       // still has data to write but pipe has no room
       ksleep(&p->room);		// sleep for room
  }
}
