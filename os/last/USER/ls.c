/********************************************************************
*Copyright: Copyright (c) 2018
*Author: Chao Jiang
*Title: ls.c
********************************************************************/
#include "ucode.c"

int printf(char *fmt,...);
char myname[128];

char buf[1024];

int ls_pathname(char *pathname,char name[])
{
    STAT stat_buf;
    u16 mode;
    int mask, k;
    stat(pathname,&stat_buf);
    mode = stat_buf.st_mode;
    if ((mode & 0xF000) == 0x4000)
        printc('d');

    if ((mode & 0xF000) == 0xA000)
        printc('l');
    else if ((mode & 0xF000) == 0x8000)
        printc('-');

    mask = 000400;
    for (k=0; k<3; k++){
      if (mode & mask)
         printc('r');
      else
         printc('-');
      mask = mask >> 1;
      if (mode & mask)
         printc('w');
      else
         printc('-');
      mask = mask >> 1;
      if (mode & mask)
         printc('x');
      else
         printc('-');
      mask = mask >> 1;
    }    
    printf(" %d ", stat_buf.st_nlink);
    printf(" %d  %d ", stat_buf.st_uid, stat_buf.st_gid);
    printf("%d ", stat_buf.st_size);
    printf("%s ", name);
    printf("\n\r");     
}

int main(int argc, char *argv[ ])
{
  DIR *dp;int i = 0;
  STAT stat_buf;
  int fd;
  char r;
  printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\r");
  printf("$                     JC's ls                     $\n\r");
  printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\r");
  char cwd[64];
  getcwd(cwd);
  if(argc == 1)
  {
	fd = open(cwd, O_RDONLY);
    stat(cwd,&stat_buf);
    strcpy(myname,cwd);
  }
  else if(argc == 2)
  {
	fd = open(argv[1], O_RDONLY);
    stat(argv[1],&stat_buf);
    strcpy(myname,argv[1]);
  }
  else
  {
	exit(0);
  }
  if((stat_buf.st_mode & 0xF000) != 0x4000)
  {
	printf("NOT DIR\n\r");
    exit(0);
  }
  //stat("/bin",&stat_buf);
  int cp = 0;
  
  while(1)
  {
  char pathname[128];
  int t = read(fd, buf, 1024);
  if(t == 0)break;
  dp = (DIR*)&buf;
  r = dp->name[dp->name_len];
  dp->name[dp->name_len] = 0;
  //printf("%s\n\r",dp->name);
  strcpy(pathname,myname);
  strcat(pathname,"/");
  strcat(pathname,dp->name);
  //printf("%s\n\r",pathname);
  ls_pathname(pathname,dp->name);
  //printf("dp->inode = %d ",dp->inode);
  //printf("dp->rec_len = %d \n\r",dp->rec_len);
  //printf("dp->name_len = %d \n\r",dp->name_len);
  dp->name[dp->name_len] = r;
  cp += dp->rec_len;
  lseek(fd,cp,SEEK_SET);
  
  }
  exit(0);
}
