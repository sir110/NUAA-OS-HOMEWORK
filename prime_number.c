#include <stdio.h>
#include <math.h> //sqrt
#include <stdlib.h> //malloc
#include <time.h>
#include <pthread.h>
#include <string.h> //memset

int *shared;
int n, count=0;

void f(int *n)
{
	int j=0, count=0;
	shared[0] = 0;
	shared[1] = 0;
	for(int i=2; i<=(int)sqrt(*n); i++)
	{
		if(shared[i])
		{
			for(int j=2*i; j<=*n; j+=i)
				shared[j] = 0;
		}
	}
}

int main()
{
	clock_t begintime = clock();
	scanf("%d", &n);
	if(n < 0) exit(0);
	shared = (int*)malloc(n*sizeof(int));
	memset(shared, 1, n*sizeof(int));

	pthread_t tid1;
	int ret1;
	ret1 = pthread_create(&tid1, NULL, (void*)f, &n);
	pthread_join(tid1, NULL);
	
	for(int j=0; j<n; j++)
	{
		if(shared[j])
		{
			printf("%d ", j);
			count++;
		}
	}
	printf("\ncount: %d", count);
	clock_t endtime = clock();
	printf("\ntime: %fs\n", (double)(endtime-begintime)/CLOCKS_PER_SEC);
	
	exit(0);
}
