#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{

	pid_t pid;

	signal(SIGUSR1,SIG_IGN);
	pid=fork();
	ppid=pid;
	switch(pid)
	{
		case -1:
			printf("fork fail\n");
			return 0;

		case 0:
			printf("child process call\n");
			execl("./reUart","reUart",0);
			printf("child process call end\n");
			break;
			
		default:
			printf("parent process call\n");
			execl("./sigact_test","sigact_test",0);
			printf("parent process call end\n");
			break;
	
	}

}
