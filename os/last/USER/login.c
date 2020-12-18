/********************************************************************
*Copyright: Copyright (c) 2018
*Author: Chao Jiang
*Title: login.c
********************************************************************/
#include "ucode.c"
int in, out, err;
char username[128];
char password[128];
char buf[1024];
char info[64];
int len;
char *list[32];
char finalcmd[128];
void tokenize(char *line)
{ int i = 0;
  char *cp;
  cp = line;
  len = 0;
  
  while (*cp != 0){
       while (*cp == ':') *cp++ = 0;        
       if (*cp != 0)
           list[len++] = cp;         
       while (*cp != ':' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  list[len] = 0;

  //for(i=0;i<len;i++)
	//{printf("%s ",list[i]);}	
}
int main(int argc, char* argv[])
{
	
    int i = 0, j = 0, k = 0;int flag = 0;
    //close file descriptors 0,1 inherited from INIT.
    close(0);
    close(1);
    //open argv[1] 3 times as in(0), out(1), err(2).
    in = open(argv[1], 0);
    out = open(argv[1], 1);
    err = open(argv[1], 2);
    //set tty name string in PROC.tty
    settty(argv[1]);
    //open /etc/passwd file for READ;
    int fd = open("/etc/passwd", O_RDONLY);
    read(fd, buf, 1024);
    //list[0] = &buf[0];
    //tmp = &buf[0];
    int last = 0;
	
    while (1) {
        printf("This is JC's login!!!\n\r");
        //printf("%s\n\r",buf);
        printf("username:");
        gets(username);
        printf("password:");
        gets(password);
        for (i = 0; i < 1024; i++) {
            if (buf[i] == 0) {
                break;
            }
            if (buf[i] == '\n') {
                
                for (k = last; k <= i; k++) {
					//printc(buf[k]);
                    info[j] = buf[k];
					//printc(info[j]);
					j++;
                }
				info[j]=0;
				//printf("%s",info);
				tokenize(info);
				//printf("unam = %s ",list[0]);
				//printf("pass = %s ",list[1]);
				if(strcmp(username,list[0]) == 0 && strcmp(password,list[1]) == 0)
				{flag = 1;break;}
				j = 0;
                last = i + 1;
            }
    	}
		//printf("\nflag = %d",flag);
		last = 0;
		if(flag)
        { 
			chuid(atoi(list[3]), atoi(list[2]));
			chdir(list[5]);
			//printf("pass = %s",list[6]);
			close(fd);
			printf("Welcome to JC's OS for CPT_S 460\n\r");
            strcpy(finalcmd,"mysh");
			strcat(finalcmd," ");
			strcat(finalcmd,list[2]);
			//printf("finalcmd=%s\n\r",finalcmd);	
			exec(finalcmd);
		}
		else
		{printf("login failed, try again\n\r");}

        
    }
}
