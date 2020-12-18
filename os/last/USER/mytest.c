/********** test.c file *************/
#include "ucode.c"
int printf(char *fmt,...);
#define BUFFSIZE 1
#define MAXLINE 1024
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

	return i;
}


int main(int argc, char *argv[ ])
{
	int fd,k;
	fd = open("/f1", O_RDONLY);//first line
	k = readline(fd);
	close(fd);
    
	fd = open("/f1", O_RDONLY);//second line
	lseek(fd,k+1,SEEK_SET);
	k = readline(fd);
    

}
