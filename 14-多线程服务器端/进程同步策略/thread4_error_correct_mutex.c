#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100

void * thread_inc(void * arg);
void * thread_des(void * arg);

long long num = 0;
pthread_mutex_t mutex;

int main(int argc, char * argv[])
{
    pthread_t thread_id[NUM_THREAD];
    int i;

    printf("sizeof long long: %ld \n", sizeof(long long));

    pthread_mutex_init(&mutex, NULL);

    for (i = 0; i < NUM_THREAD; i++)    // 这个循环内会存在并行
    {
        if(i % 2)   pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else    pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
    }
    
    for(i = 0; i < NUM_THREAD; i++) pthread_join(thread_id[i], NULL);

    printf("result: %lld \n", num);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void * thread_inc(void * arg)
{
    // 注意看两个线程的互斥量放置的位置
    // 放在循环外面是为了尽可能减少互斥量lock、unlock函数的调用次数
    // 两个位置只为相互对比，放里面耗时即长
    pthread_mutex_lock(&mutex); 
    for (int i = 0; i < 50000000; i++)
    {
        num += 1;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void * thread_des(void * arg)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 50000000; i++)
    {
        num -= 1;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
