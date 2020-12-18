/********************************************************************
*Copyright: Copyright (c) 2018
*Author: Chao Jiang
*Title: init.c
********************************************************************/
#include "ucode.c"
int console1, console2,console3;

int parent()			
{
	
  int pid, status;
  while(1){
      printf("INIT : wait for ZOMBIE child\n");
      pid = wait(&status);

    
	if (pid==console1){ 			
          printf("INIT: forks a new console login\n");
          console1 = fork(); 		
          if (console1)
              continue;
          else
              exec("login /dev/tty0"); 	
     }
		
	if (pid==console2){ 			
          printf("INIT: forks a new console login\n");
          console2 = fork(); 		
          if (console2)
              continue;
          else
              exec("login /dev/ttyS0"); 	
     }	
	
     if (pid==console3){ 			
          printf("INIT: forks a new console login\n");
          console3 = fork(); 		
          if (console3)
              continue;
          else
              exec("login /dev/ttyS1"); 	
     }

     printf("INIT: I just buried an orphan child proc %d\n", pid);

  }
}
int main()
{

  
  //int in1, in2, in3, out1, out2, out3;		// file descriptors for terminal I/O
  
  //in1 = open("/dev/tty0", O_RDONLY); 		// file descriptor 0
  //in2 = open("/dev/ttyS0", O_RDONLY);
  //in3 = open("/dev/ttyS1", O_RDONLY);
  //out1 = open("/dev/tty0", O_WRONLY); 		// for display to console
  //out2 = open("/dev/ttyS0", O_WRONLY);
  //out3 = open("/dev/ttyS1", O_WRONLY);
  //printk("Starting INIT *******\n\r");
  //printf("INIT : fork a login proc on console\n\r");

  console1 = fork();
  
  

  if(console1)				
      tswitch();
  else					
      exec("login /dev/tty0");

  console2 = fork();
  if(console2)				
      tswitch();
  else					
      exec("login /dev/ttyS0");

  console3 = fork();
  if(console3)				
      parent();
  else					
      exec("login /dev/ttyS1");

}
