#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char * argv[])
{
    pthread_t t_id; // 
    int thread_param = 5;
    //                                         这里是传递给thread_main的参数地址
    if(pthread_create(&t_id, NULL, thread_main, (void*) &thread_param) != 0)
    {
        puts("pthread_create() error");
        return -1;
    }

    sleep(10);  // 延迟线程的终止时间 
    puts("end of main");
    return 0;
}

void* thread_main(void *arg)    // arg即&thread_param
{
    int i;
    int cnt = *((int*)arg);     // 转化回int指针，并且取值
    for (i = 0; i < cnt; i++)
    {
        sleep(1);
        puts("running thread");
    }
    return NULL;
}