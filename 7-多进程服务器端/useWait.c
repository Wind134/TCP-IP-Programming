#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;
    pid_t pid = fork();

    if(pid == 0)    return 3;   // 子进程返回3
    else
    {
        printf("Child PID: %d \n", pid);    // 如果是父进程则返回分配的子进程的ID
        pid = fork();   // 然后继续分配子进程
        if (pid == 0)   exit(7);    // exit返回7
        else
        {
            printf("Child PID: %d \n", pid);
            wait(&status);  // wait函数将之前终止的子进程相关信息保存到status变量， 同时相关子进程被完全销毁

            // if判断子进程是否正常终止
            if(WIFEXITED(status))   printf("Child send one: %d \n", WEXITSTATUS(status));

            // 由于创建了两个子进程，所以需要再次调用wait函数和宏
            wait(&status);
            if(WIFEXITED(status))   printf("Child send two: %d \n", WEXITSTATUS(status));
            sleep(30);
        }
    }
    return 0;
}