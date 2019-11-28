#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

#define N 10

int buffer=0;

pthread_mutex_t mutex;
pthread_cond_t wait_cond;

void *T(void *arg){
    int i;
    int item;

    int *param=(int *)arg;

    pthread_mutex_lock(&mutex);
    while((*param)!=buffer){
            pthread_cond_wait(&wait_cond,&mutex);
    }
    printf("线程▼:T%d   Put:%d\n",(*param)+1,(*param)+1);
    buffer=(buffer+1)%N;
    pthread_cond_broadcast(&wait_cond);
    pthread_mutex_unlock(&mutex);

}


int main(){
    pthread_t pid[N];
    int j[N];
    int i;

    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&wait_cond,NULL);

    for(i=0;i<N;i++){
            j[i]=i;
            pthread_create(&pid[i],NULL,T,(void*)&j[i]);
    }

    for(i=0;i<N;i++)
            pthread_join(pid[i],NULL);

    return 0;
}