#include "ucode.c"

int main(int argc, char* argv[])
{
    int pid, status;char line[64]; 
    while (1) {
        //display executable commands in /bin directory
        //prompt for a command line cmdline = "cmd a1 a2 .... an"
        exemenu();
        //printf("input a command : ");
        ugetline(line); 
        if (!strcmp(line, "exit"))
            uexit(0);
        if (!strcmp(line, "u1"))
            ukfork("/bin/u1");
        if (!strcmp(line, "u2"))
            ukfork("/bin/u2");
        // fork a child process to execute the cmd line
        pid = fork();
        if (pid){
			// parent sh waits for child to die
            pid = wait(&status);
		}
            
        else{
			// child exec cmdline
            uexec();
        // exec("cmd a1 a2 ... an");
		}
            
    }
}
