#include "ucode.c"

int main(int argc, char* argv[])
{
    int sh, pid, status;
    sh = ufork();
    if (sh) {
        // P1 runs in a while(1) loop
        while (1) {
            pid = wait(&status); // wait for ANY child to die
            if (pid == sh) {
                // if sh died, fork another one
                sh = ufork();
                continue;
            }
            uprintf("P1: I just buried an orphan %d\n", pid);
        }
    } else
	{
		//uprintf("P1: 666\n");
		exec("sh");
	}
        
    // child of P1 runs sh
}
