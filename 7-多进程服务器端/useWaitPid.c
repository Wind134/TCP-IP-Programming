#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;
    pid_t pid = fork();

    if(pid == 0)
    {
        //sleep(15);  // 调用sleep函数推迟子进程的执行，这会导致程序延迟15s
        return 24;
    }
    else
    {
        // 下面第一个参数设置为-1，表明可以等待任意子进程的终止
        // 此外若有子进程，waitpid返回的是子进程的信息，子进程在执行的状态下则返回0
        while (!waitpid(-1, &status, WNOHANG))  
        {
            sleep(3);   // 沉睡3s是为了防止等待的时间内一直打印下行信息
            puts("sleep 3 seconds.");
        }
        
        if(WIFEXITED(status))   printf("Child send: %d \n", WEXITSTATUS(status));
    }
    return 0;
}