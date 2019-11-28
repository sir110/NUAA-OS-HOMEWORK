#include <stdio.h>
#include <sys/ipc.h> //shmget
#include <sys/shm.h> //shmget
#include <stdlib.h> //exit
#include <unistd.h> //fork
#include <sys/types.h> //wait
#include <sys/wait.h> //wait

int share[100];
int fib(int n);

int main()
{
	int n = 0;
	void *shm = NULL;
	int *shared;
	scanf("%d", &n);
	if(n < 0)
	{
		printf("error data\n");
		exit(0);
	}

	int shmid; //共享内存标识符
	shmid = shmget((key_t)1234, sizeof(share), 0666|IPC_CREAT);
	if(shmid == 1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(1);
	}
	shm = shmat(shmid, 0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(1);
	}

	shared = (int*)shm;

	pid_t pid;
	pid = fork();
	if(pid == 0)
	{
		for(int i=0; i<n; i++)
		{
			shared[i] = fib(i);
		}
	}
	wait(NULL);
	if(pid != 0)
	{
		for(int i=0; i<n; i++)
			printf("%d ", shared[i]);
		printf("\n");
	}

	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(1);
	}
	if(shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(1);
	}
	return 0;
}

int fib(int n)
{
	if(n == 0)
		return 0;
	if(n == 1)
		return 1;
	return fib(n-1) + fib(n-2);
}



