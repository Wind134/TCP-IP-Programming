#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void * read(void * arg);
void * accu(void * arg);

static sem_t sem_one;   // 用作同步机制
static sem_t sem_two;   // 即用作处理先后顺序的机制
static int num;

int main(int argc, char * argv[])
{
    pthread_t id_t1, id_t2;

    // 初始信号量为0，这就意味着
    // 先调用该信号量进入临界区的线程必然是后执行的线程
    sem_init(&sem_one, 0, 0);
    
    // 初始信号量为1，意味着这是正常先执行的线程
    sem_init(&sem_two, 0, 1);

    pthread_create(&id_t1, NULL, read, NULL);
    pthread_create(&id_t2, NULL, accu, NULL);

    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);

    sem_destroy(&sem_one);
    sem_destroy(&sem_two);
    return 0;
}

void * read(void * arg)
{
    
    for (int i = 0; i < 5; i++)
    {
        fputs("Input num: ", stdout);
        sem_wait(&sem_two); // sem_two信号量减1
        scanf("%d", &num);
        sem_post(&sem_one); // sem_one信号量加1
    }
    return NULL;
}

void * accu(void * arg)
{
    int sum = 0;
    
    for (int i = 0; i < 5; i++)
    {
        sem_wait(&sem_one); // sem_one信号量+1
        sum += num;
        sem_post(&sem_two); // sem_two信号量-1
    }

    printf("Result: %d \n", sum);
    return NULL;
}