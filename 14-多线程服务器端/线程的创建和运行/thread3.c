#include <stdio.h>
#include <pthread.h>
void * thread_summation(void * arg);
int sum = 0;    // 全局变量，每个函数都能共享该变量

int main(int argc, char * argv[])
{
    pthread_t id_t1, id_t2;
    int range1[] = {1, 5};
    int range2[] = {6, 10};

    pthread_create(&id_t1, NULL, thread_summation, (void *)range1); // 创建线程1
    pthread_create(&id_t2, NULL, thread_summation, (void *)range2); // 创建线程2

    pthread_join(id_t1, NULL);  // 阻塞，直到线程1执行完毕
    pthread_join(id_t2, NULL);  // 阻塞，直到线程2执行完毕

    printf("result: %d \n", sum);
    return 0;
}

void * thread_summation(void * arg)
{
    int start = ((int *)arg)[0];
    int end = ((int *)arg)[1];

    while (start <= end)
    {
        sum += start;
        start++;
    }
    return NULL;
}
