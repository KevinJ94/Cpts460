/********************************************************************
*Copyright: Copyright (c) 2018
*Author: Chao Jiang
*Title: mymore.c
********************************************************************/
#include "ucode.c"

#define PAGELEN 15
#define LINELEN 60
int printf(char *fmt,...);
extern char *tmp_path;
int std_in_redirect=1;
int std_out_redirect=1;

void do_more(int fd)
{
    char buf[LINELEN];
    int num_of_lines = 0;
    int reply;
    int read_num;
    while (1)
    {
		read_num = read(fd, buf, LINELEN);
		if(read_num <= 0)return 0;

        if (num_of_lines == PAGELEN)
        {
            reply = see_more();
            if(reply == 0)
                break;
            num_of_lines -= reply;
        }

        if (write(1, buf, read_num) < 0)
            exit(1);
        num_of_lines++;
    }
}

int see_more()
{
    int c;
	char cmd;
    //printf("\033[7m more? \033[m");
    while ((c = getc(cmd)) != EOF)
    {
        if (c == 'q')
            return 0;
        if (c == ' ')
            return PAGELEN;
        if (c == '\r')
            return 1;
    }
    return 0;
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
    printf("$                    JC's MORE                    $\n\r");
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\r");
    if (argc == 1)
        {do_more(0);exit(0);}
    else if(argc == 2)
		{fd = open(argv[1], O_RDONLY);do_more(fd);}
    return 0;

}
