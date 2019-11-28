#include <stdio.h>
#include <stdlib.h> //malloc
#include <semaphore.h> //sem_wait, sem_post
#include <pthread.h> //pthread
#include <unistd.h> //sleep

int writer_count, reader_count;
sem_t lock, write_lock, read_lock, wrt, mutex3;

struct data
{
	int id;
	int arrive; //arrive time
	int op_time; //operate time
};

void reader();
void writer();

int main()
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	sem_init(&lock, 0, 1);
	sem_init(&write_lock, 0, 1);
	sem_init(&read_lock, 0, 1);
	sem_init(&wrt, 0, 1);
	sem_init(&mutex3, 0, 1);

	reader_count = 0;
	writer_count = 0;

	int w, r;
	printf("读者数目：");
	scanf("%d", &r);
	printf("写者数目：");
	scanf("%d", &w);
	struct data *d = (struct data*)malloc((w+r)*sizeof(struct data));
	for(int i=0; i<r; i++)
	{
		printf("读者%d:", i+1);
		int arrive, op_time;
		scanf("%d%d", &arrive, &op_time);
		d[i].id = i+1;
		d[i].arrive = arrive;
		d[i].op_time = op_time;
	}
	for(int j=0; j<w; j++)
	{
		printf("写者%d:", j+1);
		int arrive, op_time;
		scanf("%d%d", &arrive, &op_time);
		d[r+j].id = j+1;
		d[r+j].arrive = arrive;
		d[r+j].op_time = op_time;
	}
	pthread_t tid[r+w];
	for(int i=0; i<r; i++)
	{
		pthread_create(&tid[i], &attr, (void*)reader, d++);
	}
	for(int j=r; j<r+w; j++)
	{
		pthread_create(&tid[j], &attr, (void*)writer, d++);
	}
	for(int i=0; i<r+w; i++)
	{
		pthread_join(tid[i], NULL);
	}
	sem_destroy(&lock);
	sem_destroy(&write_lock);
	sem_destroy(&read_lock);
	sem_destroy(&wrt);
	sem_destroy(&mutex3);
	return 0;
}

void writer(void *d)
{
	int id = ((struct data*)d)->id;
	int arrive = ((struct data*)d)->arrive;
	int op_time = ((struct data*)d)->op_time;
	sleep(arrive);

	sem_wait(&write_lock);
	++writer_count;
	if(writer_count == 1)
		sem_wait(&lock);
	sem_post(&write_lock);

	sem_wait(&wrt);
	printf("writer %d is writing\n", id);
	sleep(op_time);
	printf("writer %d is leaving\n", id);
	sem_post(&wrt);

	sem_wait(&write_lock);
	--writer_count;
	if(writer_count == 0)
		sem_post(&lock);
	sem_post(&write_lock);
}

void reader(void *d)
{
	int id = ((struct data*)d)->id;
	int arrive = ((struct data*)d)->arrive;
	int op_time = ((struct data*)d)->op_time;
	sleep(arrive);

	sem_wait(&mutex3);
	sem_wait(&lock);
	sem_wait(&read_lock);
	++reader_count;
	if(reader_count == 1)
		sem_wait(&wrt);
	sem_post(&read_lock);
	sem_post(&lock);
	sem_post(&mutex3);

	printf("reader %d is reading\n", id);
	sleep(op_time);
	printf("reader %d is leaving\n", id);

	sem_wait(&read_lock);
	--reader_count;
	if(reader_count == 0)
		sem_post(&wrt);
	sem_post(&read_lock);
}

