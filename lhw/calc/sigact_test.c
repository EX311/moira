#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void cal_process_start(int sig)
{
	printf("cal process start\n");
	pid_t pid;

	pid=fork();
	switch(pid)
	{
		case -1:
			printf("cal process fork fail\n");
			break;

		case 0:
			printf("fork calc start\n");
			execl("./calc_proc","calc_proc",0);
			printf("fork calc end\n");
			break;
			
		default:
			break;
	
	}

}

int main()
{
	struct sigaction act;
	act.sa_handler=cal_process_start;
	sigaction(SIGUSR1, &act, 0);
	while(1)
	{
//		printf("USER sigal wait!!!\n");
		sleep(1);
	}

}
