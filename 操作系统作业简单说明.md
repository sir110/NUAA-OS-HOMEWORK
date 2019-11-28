# 简单说明

## 作业1

* **题目：**

Fibonacci序列是一组数：`0，1，1，2，3，5，8，...，`它可以表达为：

fib(0) = 0

fib(1) = 1

fib(n) = fib(n-1) + fib(n-2)

使用fork()编写一个程序，接受用户在命令行中输入一个数字n，输出Fibonacci序列中的前n个数。

要求：使用shmget在父进程与子进程之间建立一个共享内存段，允许子进程将Fibonacci序列的内容写入共享内存段，父进程等待子进程，当子进程完成后，父进程输出Fibonacci序列。

参考步骤如下：
a.接受命令行上传递的参数，执行错误检查
b.创建一个共享内存段
c.将共享内存连接到地址空间
d.创建子进程，调用wait()等待子进程结束
e.子进程计算fibonacci序列，写入共享内存，最后断开此区域
f.父进程在终端上输出共享内存段中的fibonacci序列的值
g.断开并删除内存共享段

* **思路**

使用共享内存段，子进程写入共享内存段，父进程输出序列。

* **编译程序**

编译链接

```shell
cc fibonacci.c
```

运行

```shell
./a.out
```

输入一个数字n，输出Fibonacci序列中的前n个数

```shell
10
0 1 1 2 3 5 8 13 21 34
```

* **程序说明**

```c
/*共享内存的初始化与使用*/
void *shm = NULL;
int shmid; //共享内存标识符
shmid = shmget((key_t)1234, sizeof(share), 0666|IPC_CREAT); //创建共享内存
if(shmid == -1) //错误检查
{
	fprintf(stderr, "shmat failed\n");
    exit(1);
}
shm = shmat(shmid, 0, 0); //共享内存连接到当前进程的地址空间
if(shm == (void*)-1) //错误检查
{
    fprintf(stderr, "shmat failed\n");
    exit(1);
}
shared = (int*)shm;
```



## 作业2

* **题目**

用`pthread`编写一个多线程程序来输出素数。程序应该这样工作：用户输入运行程序时在命令行输入一个数字，然后输出小于或等于用户输入数的所有素数。请大家思考一下如何利用多线程提高计算速度，希望越快越好（注：不可以在程序中预存结果，最后会公布各位同学程序运行速度的排名）。

* **思路**

编写多线程来输出素数，素数采用线性筛法进行识别。

* **编译程序**

编译链接

```shell
cc pthread.c -lpthread -lm
```

运行

```shell
./a.out
```

输入一个数字，然后输出小于或等于用户输入数的所有素数

```c
100
2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 
count: 25 //素数数目
time: 0.001055s //运行时间
```

* **程序说明**

```c
/*素数的筛选函数*/
int *shared; //shared中保存着素数，0表示合数，1表示素数
void f(int *n)
{
    int j=0, count=0;
    shared[0] = 0; //合数
    shared[1] = 0; //合数
    for(int i=2; i<=(int)sqrt(*n); i++)
    {
        if(shared[i]) //把素数的倍数全部筛出去
        {
            for(int j=2*i; j<=*n; j+=i)
                shared[j] = 0;
        }
    }
}

```



## 作业3

* **题目**

解决我们在课堂上学到的读者和写者问题中有利于读者, 可能会让写者饿死的情况。为读者和写者的问题提供另一种有利于写者的解决方案, 即一旦写者等待写入共享数据, 就不会有新的读者开始阅读。使用线程同步编写解决方案。可以自由地使用锁、信号量等来实现。

* **思路**

如果有一个写者在等待，则新到来的读者不允许进行读操作。为此应当添加一个整型变量writer_count，用于记录正在等待的写者的数目，当writer_count = 0时，才可以释放等待的读者线程队列。

设置信号量mutex3避免写者同时与多个读者进行竞争，读者中信号量lock比mutex3先释放，则一旦有写者，写者可马上获得资源。即一旦写者等待写入共享数据, 就不会有新的读者开始阅读。

* **编译程序**

编译链接

```shell
cc writer_reader.c -lpthread
```

运行

```c
./a.out
```

输入id，读r或写w，以及操作时间

```shell
读者数目： 2
写者数目： 2
读者1：0 4
读者2：3 5
写者1：2 6
写者2：4 7
reader 1 is reading
reader 1 is leaving
writer 1 is writing
writer 1 is leaving
writer 2 is writing
writer 2 is leaving
reader 2 is reading
reader 2 is leaving
```



* **程序说明**

设置两个全局变量writer_count,reader_count记录读者和写者的数量

设置信号量lock,write_lock,read_lock,wrt,mutex3

信号量write_lock在写者的进入区和退出区中使用，对变量writer_count的修改进行互斥

信号量read_lock在读者的进入区和退出区中使用，对变量reader_count的修改进行互斥

信号量lock则是读者和写者两个之间的互斥信号量，保证每次只读或者只写。由于写者优先，写者的操作优于读者，则信号量一直被占用着，直到没有写者才释放。

信号量wrt保证每次只有一个写者进行写操作

信号量mutex3避免写者同时与多个读者进行竞争，读者中信号量lock比mutex3先释放，则一旦有写者，写者可马上获得资源。即一旦写者等待写入共享数据, 就不会有新的读者开始阅读。

```c
/*写者函数*/
void writer(void *d)
{
    int id = ((struct data*)d)->id;
    int op_time = ((struct data*)d)->op_time;
    sleep(op_time); //挂起op_time时间

    sem_wait(&write_lock); //信号量write_lock对变量writer_count的修改进行互斥
    ++writer_count;
    if(writer_count == 1) //第一个写者申请信号量lock，其余的写者无需再次申请
        sem_wait(&lock);  //lock保证每次只读或者只写
    sem_post(&write_lock);

    sem_wait(&wrt); //信号量wrt来保证每次只有一个写者进行写操作
    printf("Thread %d: start writing\n", id);
    sleep(1);
    printf("Thread %d: end writing\n", id);
    sem_post(&wrt);

    sem_wait(&write_lock);
    --writer_count;
    if(writer_count == 0) //最后一个写者释放信号量lock
        sem_post(&lock);
    sem_post(&write_lock);
}

```



```c
/*读者函数*/
void reader(void *d)
{
    int id = ((struct data*)d)->id;
    int op_time = ((struct data*)d)->op_time;
    sleep(op_time);

    sem_wait(&mutex3); //mutex3避免写者同时与多个读者进行竞争
    sem_wait(&lock);
    sem_wait(&read_lock); //信号量read_lock对变量reader_count的修改进行互斥
    ++reader_count;
    if(reader_count == 1)
        sem_wait(&wrt);
    sem_post(&read_lock);
    sem_post(&lock);
    sem_post(&mutex3);

    printf("Thread %d: start reading\n", id);
    sleep(1);
    printf("Thread %d: end reading\n", id);

    sem_wait(&read_lock);
    --reader_count;
    if(reader_count == 0)
        sem_post(&wrt);
    sem_post(&read_lock);
}

```

