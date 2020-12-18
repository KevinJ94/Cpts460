/********** test.c file *************/
#include "ucode.c"
int printf(char *fmt,...);
char buf[1024];
extern char *tmp_path;
int std_in_redirect=1;
int std_out_redirect=1;

#define BUFFSIZE 1
#define MAXLINE 1024
int in = 0;
int out = 1;
int err = 2;
char ttyname[32];
int recover()
{
	gettty(ttyname);
	close(0);
    close(1);
    in = open(ttyname, 0);
    out = open(ttyname, 1);
    err = open(ttyname, 2);
}

int mycat(int infd,int out)
{
	
    char buf[1024];
	char echo[MAXLINE]={0};
    int buf_size = sizeof(buf);
	int i = 0;
    int n_read = 0;
    do {
		if(i>(MAXLINE-2)){i = 0; echo[i]=0;}
        n_read = read(infd, buf, buf_size);
        if (n_read == 0)
            return 0;
        else if (n_read == -1) {
            printf("mycat() read error\n\r");
            return -1;
        }
        else {
            if (write(out, buf, n_read) < 0) {
                printf("mycat() write error\n\r");
                return -1;
            }
			//write(newfd, buf, n_read);
			echo[i] = buf[0];
			echo[i+1] = 0;
		    if(strcmp(buf,"\n")==0 || strcmp(buf,"\r")==0)
				{printf("\n\r");prints(echo);printf("\n\r");i = 0; echo[i]=0;continue;}
        }
		i++;
    }
    while (n_read > 0);
    return 0;
}

int mycat2(int infd)
{
	
    char buf[1024];
    char *cr = "\n";
    int buf_size = sizeof(buf);
    gettty(ttyname);
    out = open(ttyname, 1);
    int n_read = 0;
    do {
		
        n_read = read(infd, buf, buf_size);
        if (n_read == 0)
            return 0;
        else if (n_read == -1) {
            printf("mycat() read error 1\n\r");
            return -1;
        }
        else {
			
            if (write(1, buf, n_read) < 0) {
                printf("mycat() write error 2\n\r");
                return -1;
            }
			
            if (write(out, buf, n_read) < 0) {
                printf("mycat() write error 3\n\r");
                return -1;
            }
			
			
		    if(strcmp(buf,"\n")==0 || strcmp(buf,"\r")==0)
				{write(1, cr, 1);write(out, cr, 1);}
        }
		
    }
    while (n_read > 0);
    return 0;
}

int cat_file(int infd,int out)
{
	char buf[32];
    int buf_size = sizeof(buf);
	int lines = 0;
	int i = 0;
    int n_read = 0;
    do {
        n_read = read(infd, buf, buf_size);
        if (n_read == 0)
            {recover();printf("\n\rTotal lines: %d\n\r",lines);return 0;}
        else if (n_read == -1) {
            printf("mycat() read error\n\r");
            return -1;
        }
        else {
            if (write(out, buf, n_read) < 0) {
                printf("mycat() write error\n\r");
                return -1;
            }
			for(i=0;i<n_read;i++)
			{
					
				if(buf[i] == '\n')
					lines++;
			}
			
        }
		
    }
    while (n_read > 0);
	
    return 0;
}

int main(int argc, char *argv[ ])
{


  int i;
  int fd,n;
  int newfd = dup(1);
  char buf[5];
  int t_fd = open(tmp_path, O_RDONLY);
  if(read(t_fd, buf, 2) >= 0)
  {close(t_fd);
  if(buf[0]=='0')std_in_redirect=0;
  if(buf[0]=='1')std_in_redirect=1;
  if(buf[1]=='0')std_out_redirect=0;
  if(buf[1]=='1')std_out_redirect=1;
  if(std_out_redirect == 0){unlink(tmp_path);}
  }
  //int pid = getpid();
  //printf("JC: PROC %d running test program\n\r", pid);
  //printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\r");
  //printf("$                     JC's CAT                    $\n\r");
  //printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\r");
  //printf("argc = %d\n\r", argc);
  //for (i=0; i<argc; i++)
  //  printf("argv[%d] = %s\n\r", i, argv[i]);
  //if(!std_out){recover();printf("recover()...OK!\n\r");}

  if(std_out_redirect == 1)
  {
	if(argc == 1)
		mycat(0,newfd);
	else if(argc == 2)
		{fd = open(argv[1], O_RDONLY);cat_file(fd,1);}
  }
  else if(std_out_redirect == 0)
  {
	if(argc == 1)
		{mycat2(0);}
	else if(argc == 2)
		{fd = open(argv[1], O_RDONLY);cat_file(fd,1);}
  }


  exit(0);

/*
  if(argc == 1 && std_out_redirect == 1)
	mycat(0,newfd);
  else if(argc == 1 && std_out_redirect == 0)
    {mycat2(in);}
  else if(argc == 2 && std_out_redirect == 1)
	{fd = open(argv[1], O_RDONLY);cat_file(fd,1);}
  else if(argc == 2 && std_out_redirect == 0)
	{fd = open(argv[1], O_RDONLY);cat_file(fd,1);}
  else
	exit(0);
 */
}
