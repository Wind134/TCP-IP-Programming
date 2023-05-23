/* 这是一个示例，利用信号处理技术消灭僵尸进程 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);   // WNOHANG表示即便没有终止的子进程也不会进入阻塞状态
    if(WIFEXITED(status))
    {
        printf("Removed proc id: %d \n", id);
        printf("Child send: %d \n", WEXITSTATUS(status));
    }
}

int main()
{
    pid_t pid;
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);  // sa_mask成员的所有位初始化为0
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);    // 表明在子进程终止时调用act

    // 以下的代码都基于一点：子进程终止时将产生SIGCHLD信号
    pid = fork();
    if(pid == 0)
    {
        puts("Hi! I'm child process 1");
        pid = fork();   // 子进程也可以fork出新的子进程
        if (pid == 0)   puts("Hi! I'm the child of the child process 1");
        else
        {
            printf("Child of the child proc id: %d \n", pid);
            sleep(10);
            return 12;
        }
    }
    else    // 仅有父进程会执行这一段
    {
        printf("Child proc id: %d \n", pid);
        pid = fork();

        if(pid == 0)    // 这是新的子进程执行的
        {
            puts("Hi! I'm child process 2");
            sleep(10);
            return 24;
        }
        else    // 仅有父进程会执行这一段
        {
            printf("Child proc id: %d \n", pid);
            for (int i = 0; i < 6; i++)
            {
                puts("wait...");
                sleep(5);
            }
        }
    }
    return 0;
}