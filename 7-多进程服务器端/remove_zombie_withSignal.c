/* 这是一个示例，利用信号处理技术消灭僵尸进程 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
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
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid = fork();
    if(pid == 0)
    {
        puts("Hi! I'm child process 1");
        sleep(10);
        return 12;
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