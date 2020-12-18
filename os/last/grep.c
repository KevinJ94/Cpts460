/********** test.c file *************/
#include "ucode.c"
#define BUFFSIZE 1
#define MAXLINE 2048

int printf(char *fmt,...);
extern char *tmp_path;
int std_in_redirect=1;
int std_out_redirect=1;

char maxline[MAXLINE];



int readline(int fd)
{
	char buf[BUFFSIZE];
	
    int i=0,count=0;
	while(1)
	{
		count = read(fd, buf, BUFFSIZE);
		if(buf[0] == 0 || buf[0] == '\n' || buf[0] == EOF)
		{
			maxline[i] = buf[0];
			maxline[i+1] = 0;
			i++;
			break;
		}
		else
		{
			maxline[i] = buf[0];
		}
		
		lseek(fd,0,SEEK_CUR);
		if(count <= 0)
			{return 0;}
		i++;
	}
	
	return count;
}



int do_search(int fd,char keywords[])
{
	
	int read_count = 0;
	char tmp[MAXLINE];
	while(1)
	{
		read_count = readline(fd);
		if(read_count <= 0)break;
		strcpy(tmp,maxline);
		//printf("%s",&tmp[0]);
		if(strstr(&tmp[0],&keywords[0])!=0)
			printf("%s",tmp);
	}
}



int main(int argc, char *argv[ ])
{
	int fd;
	char buf[5];
  	int t_fd = open(tmp_path, O_RDONLY);
  	if(read(t_fd, buf, 2) >= 0)
  	{	close(t_fd);
  		if(buf[0]=='0')std_in_redirect=0;
  		if(buf[0]=='1')std_in_redirect=1;
  		if(buf[1]=='0')std_out_redirect=0;
  		if(buf[1]=='1')std_out_redirect=1;
  		if(std_out_redirect == 0){unlink(tmp_path);}
  	}
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\r");
    printf("$                    JC's GREP                    $\n\r");
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\r");
    if (argc == 1)
        {printf("bad usage\n\r");exit(0);}
    else if(argc == 2)
		{do_search(0,argv[1]);exit(0);}
	else if(argc == 3)
		{fd = open(argv[1], O_RDONLY);do_search(fd,argv[2]);exit(0);}
    return 0;

}
