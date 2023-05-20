/* 展示僵尸进程的创建 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t pid = fork();

    if(pid == 0)    // 如果是子进程
        puts("Hi, I am a child process");
    else    // 如果是父进程
    {
        printf("Child Process ID: %d \n", pid);
        sleep(30);  // 父进程暂停30s
        /* 运行程序时，由于父进程暂停，此时ps au查询进程信息，可以看到有一个为僵尸进程的子进程 */
    }
        
    if (pid == 0)   puts("End child process");
    else    puts("Ends parent process");

    return 0;    
}