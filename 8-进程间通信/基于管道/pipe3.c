/* 
这个例子是一个通过管道双向通信的展示
避免了pipe2中的问题，不会存在子进程先读取了父进程应该读取的数据的可能
因为两个进程分别控制不同管道的出口，分别控制不同管道的入口
 */
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main()
{
    int fds1[2], fds2[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message.";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds1), pipe(fds2);
    pid = fork();

    // 对于子进程，控制管道1的入口，管道2的出口
    if (pid == 0)
    {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    // 对于父进程，控制管道2的入口，管道1的出口
    else
    {
        read(fds1[0], buf, BUF_SIZE);
        printf("Parent proc output: %s \n", buf);
        write(fds2[1], str2, sizeof(str2));
        sleep(3);   // 如果不延迟3s(2、1)，父进程执行结束之后直接return 0，子进程都来不及read了
    }
    return 0;
}