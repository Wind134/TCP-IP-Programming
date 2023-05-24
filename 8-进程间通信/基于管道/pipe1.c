/* 这个例子是一个简单的通过管道通信的展示
其中子进程仅使用输入路径，而父进程仅使用输出路径
 */
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main()
{
    int fds[2];
    char str[] = "Who are you?";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds);
    pid = fork();

    // 对于子进程，进行写入
    if (pid == 0)
        write(fds[1], str, sizeof(str));
    // 对于父进程，进行读取
    else
    {
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }
    return 0;
}