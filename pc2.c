#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

#define CAPACITY 4

char buffer1[4];
char buffer2[4];

int crea; //生产者 
int comp1;//计算者 
int comp2;//计算者 
int cons; //消费者 


int buffer1_is_empty(){
    return crea==comp1;
}

int buffer1_is_full(){
    return (crea+1)%CAPACITY==comp1;
}

int buffer2_is_empty(){
    return comp2==cons;
}

int buffer2_is_full(){
    return (cons+1)%CAPACITY==comp2;
}

int get_item1(){
    int item;

    item=buffer1[comp1];
    comp1=(comp1+1)%CAPACITY;
    return item;
}

int get_item2(){
    int item;

    item=buffer2[cons];
    cons=(cons+1)%CAPACITY;
    return item;
}

int put_item1(int item){
        buffer1[crea]=item;
        crea=(crea+1)%CAPACITY;
}

int put_item2(int item){
        buffer2[comp2]=item;
        comp2=(comp2+1)%CAPACITY;
}

typedef struct{
        int value;
        pthread_mutex_t mutex;
        pthread_cond_t  cond;
}sema_t;

void sema_init(sema_t *sema,int value){
        sema->value=value;
        pthread_mutex_init(&sema->mutex,NULL);
        pthread_cond_init(&sema->cond,NULL);
}

void sema_wait(sema_t *sema){
        pthread_mutex_lock(&sema->mutex);
        sema->value--;
        while(sema->value<0)
                pthread_cond_wait(&sema->cond,&sema->mutex);
        pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema){
        pthread_mutex_lock(&sema->mutex);
        ++sema->value;
        pthread_cond_signal(&sema->cond);
        pthread_mutex_unlock(&sema->mutex);
}

sema_t mutex_sema1;
sema_t empty_buffer1_sema;
sema_t full_buffer1_sema;

sema_t mutex_sema2;
sema_t empty_buffer2_sema;
sema_t full_buffer2_sema;

#define ITEM_COUNT  (CAPACITY *2)

void *consumer(void *arg){
    int i;
    int item;

    for(i=0;i<ITEM_COUNT;i++){
            sema_wait(&full_buffer2_sema);
            sema_wait(&mutex_sema2);

            item=get_item2();

            sema_signal(&mutex_sema2);
            sema_signal(&empty_buffer2_sema);
            printf("                   consume item:%c\n",item);
    }
    return NULL;
}

void *computer(void *arg){
    int i;
    int item;

    for(i=0;i<ITEM_COUNT;i++){
            sema_wait(&full_buffer1_sema);
            sema_wait(&mutex_sema1);

            item=get_item1();

            sema_signal(&mutex_sema1);
            sema_signal(&empty_buffer1_sema);
            printf("        computer get item:%c\n",item);

            sema_wait(&empty_buffer2_sema);
            sema_wait(&mutex_sema2);

            item-=32;
            put_item2(item);
            printf("        computer put item:%c\n",item);

            sema_signal(&mutex_sema2);
            sema_signal(&full_buffer2_sema);
    }
    return NULL;
}

void create(){
        int i;
        int item;

        for(i=0;i<ITEM_COUNT;i++){
            sema_wait(&empty_buffer1_sema);
            sema_wait(&mutex_sema1);

            item=i+'a';
            put_item1(item);
            printf("create item:%c\n",item);

            sema_signal(&mutex_sema1);
            sema_signal(&full_buffer1_sema);
        }
}

int main(){
        pthread_t computer_tid;
        pthread_t consumer_tid;

        sema_init(&mutex_sema1,1);
        sema_init(&empty_buffer1_sema,CAPACITY-1);
        sema_init(&full_buffer1_sema,0);

        sema_init(&mutex_sema2,1);
        sema_init(&empty_buffer2_sema,CAPACITY-1);
        sema_init(&full_buffer2_sema,0);

        pthread_create(&consumer_tid,NULL,consumer,NULL);
        pthread_create(&computer_tid,NULL,computer,NULL);
        create();

        pthread_join(consumer_tid,NULL);
        pthread_join(computer_tid,NULL);
        return 0;
}