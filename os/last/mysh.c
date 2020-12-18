#include "ucode.c"

int len;
char* list[32];
int in = 0;
int out = 1;
int err = 2;
char ttyname[32];
int std_in_redirect = 1;
int std_out_redirect = 1;

extern char *tmp_path;
extern char *tmp_pathname;
struct parse_info {
    int flag;
    char* in_file;
    char* out_file;
    char* command2;
    char* parameters2;
};

int parse_info_init(struct parse_info *info)
{
    info->flag = 0;
    info->in_file = 0;
    info->out_file = 0;
    info->command2 = 0;
    info->parameters2 = 0;
    return 0;
}

int creat_tmp()
{
	char buf[128];
//save std
	int t_fd = open(tmp_path, O_WRONLY|O_CREAT);
	buf[0] = std_in_redirect + '0';
	buf[1] = std_out_redirect + '0';
	write(t_fd, buf, 2);
	close(t_fd);

    
}

int recover()
{
	close(0);
    close(1);
    in = open(ttyname, 0);
    out = open(ttyname, 1);
    err = open(ttyname, 2);
}
void tokenize(char* line)
{

    int i = 0;
    char* cp;
    cp = line;
    len = 0;

    while (*cp != 0) {
        while (*cp == ' ')
            *cp++ = 0;
        if (*cp != 0)
            list[len++] = cp;
        while (*cp != ' ' && *cp != 0)
            cp++;
        if (*cp != 0)
            *cp = 0;
        else
            break;
        cp++;
    }
    list[len] = 0;

    //for (i = 0; i < len; i++) {
    //    printf("%s\n\r", list[i]);
    //}
}

int parsing(struct parse_info *info)
{
	int i;
	parse_info_init(info);
    for (i = 0; i < len;) {
        if(!strcmp(list[i], "<") || strcmp(list[i],"<<")==0) 
	    {
			//printf("info->flag |= IN_REDIRECT;\n\r");
			info->flag |= IN_REDIRECT;
            info->in_file = list[i+1];
            list[i] = 0;
            i+=2;
		}
		else if(strcmp(list[i],">")==0)
		{
			//printf("info->flag |= OUT_REDIRECT;\n\r");
			info->flag |= OUT_REDIRECT;
            info->out_file = list[i+1];
            list[i] = 0;
            i+=2;
		} else if(strcmp(list[i],">>")==0)
        {
			//printf("info->flag |= OUT_REDIRECT_APPEND;\n\r");
            info->flag |= OUT_REDIRECT_APPEND;
            info->out_file = list[i+1];
            list[i] = 0;
            i+=2;
        }
        else if(strcmp(list[i],"|")==0)
        {
			//printf("info->flag |= IS_PIPED;\n\r");
            char* pCh;
            info->flag |= IS_PIPED;
            list[i] = 0;
            info->command2 = list[i+1];
            info->parameters2 = list[i+2];
            
            break;
        }
		else
			i++;
		
    }
}

int main(int argc, char* argv[])
{
    char mybuf[2048];
    int i;
    int in, out, err;
    int pid,pid2, status;
    char line[64];
    char cmd[64];
	char finalcmd[64];
	
    int fd;
	int pipe_fd[2],in_fd,out_fd;
	struct parse_info info;
    //for (i=0; i<argc; i++)
    //	printf("argv[%d] = %s\n\r", i, argv[i]);
    while (1) {
        //close(0);
        //close(1);
		gettty(ttyname);
		in = 0;
		out = 1;
		
        exemenu(argv[1]);
        gets(line);
        strcpy(cmd, line);
        tokenize(line);

        if (list[0] == 0) {
            //printf("empty\n\r");
            continue;
        } else if (!strcmp(list[0], "cd")) {
            //printf("This is CD\n\r");
            if (list[1] == 0)
                chdir("/");
            else
                chdir(list[1]);
            continue;
        } else if (!strcmp(list[0], "pwd")) {
            //printf("This is CD\n\r");
            pwd();
            continue;
        } else if (!strcmp(list[0], "logout")) {
            printf("This is logout\n\r");
			//recover();
            exit(0);
            //tswitch();
        } else {
			parsing(&info);
			/*  
		else {

            // fork a child process to execute the cmd line
            pid = fork();
            if (pid) {
                // parent sh waits for child to die
                pid = wait(&status);
            } else {
                // child exec cmdline
                exec(cmd);
                // exec("cmd a1 a2 ... an");
            }
        }
	*/
        }


		pid = fork();
		if(pid) 
        {

            pid = wait(&status);
        }
        else 
        {

            if(info.flag & IS_PIPED) //command2 is not null
            {                
                if(!(info.flag & OUT_REDIRECT) && !(info.flag & OUT_REDIRECT_APPEND)) // ONLY PIPED
	           {
					
					pipe(pipe_fd);
					pid = fork();
					if(pid) //command2
                	{
                   
						strcpy(finalcmd,info.command2);
						//if(info.parameters2[0] != 0)
						//	{strcat(finalcmd," ");strcat(finalcmd,info.parameters2[0]);}
						close(pipe_fd[1]); // close pipe WRITE end
						dup2(pipe_fd[0], 0);
						//exec(cmd2);
					if(list[len-1] != 0 && len > 4 && list[2] == 0)
						{strcat(finalcmd," ");strcat(finalcmd,list[len-1]);}
						//printf("exec 1= %s\n\r",finalcmd);
                    	exec(finalcmd);
                	}
                	else
                	{
                    	close(pipe_fd[0]); // close pipe READ end
						dup2(pipe_fd[1], 1); // redirect stdout to pipe WRITE end
						strcpy(finalcmd,list[0]);
						strcat(finalcmd," ");
						if(list[1] != 0 && len > 1)
							strcat(finalcmd,list[1]);
						if(list[2] != 0 && len > 2)
							{strcat(finalcmd," ");strcat(finalcmd,list[2]);}
						exec(finalcmd);
                	}
					
                }
                else //OUT_REDIRECT and PIPED
	           {
					recover();
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                    if(info.flag & OUT_REDIRECT)
    	               out_fd = open(info.out_file, O_WRONLY|O_CREAT);
                    else
    	               out_fd = open(info.out_file, O_WRONLY|O_APPEND);
                    close(out); 
                    dup2(out_fd, out);
                    close(out_fd);	        
                }
            }
            else
            {
                if(info.flag & OUT_REDIRECT) // NO PIPE
	           {
					
					recover();
					std_out_redirect = 0;
                    creat_tmp();
					out_fd = open(info.out_file, O_WRONLY|O_CREAT);
					if(out_fd < 0) {printf("open failed\n\r");continue;}
                    close(out); 
                    dup2(out_fd, out);
                    close(out_fd);
                    
               }
                if(info.flag & OUT_REDIRECT_APPEND)
	           {
					
					recover();
                    std_out_redirect = 0;
                    out_fd = open(info.out_file, O_WRONLY|O_APPEND);
					if(out_fd < 0) {printf("open failed\n\r");continue;}
                    close(out); 
                    dup2(out_fd, out);
                    close(out_fd);
                }
            }
            
            if(info.flag & IN_REDIRECT)
            {
				recover();
                in_fd = open(info.in_file, O_CREAT|O_RDONLY);
                close(in); 
                dup2(in_fd, in);
                close(in_fd); 
            }
			strcpy(finalcmd,list[0]);
			strcat(finalcmd," ");
			if(list[1] != 0 && len > 1)
				strcat(finalcmd,list[1]);
			if(list[2] != 0 && len > 3)
				{strcat(finalcmd," ");strcat(finalcmd,list[2]);}
			if(list[1] != 0 && list[2] != 0 && len > 2 )
				{strcat(finalcmd," ");strcat(finalcmd,list[2]);}
            //printf("exec 2= %s\n\r",finalcmd);

            exec(finalcmd);
        }		

  


        
    }
}
