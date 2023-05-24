/* 
这个例子是一个通过管道双向通信的展示
 */
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main()
{
    int fds[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message.";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds);
    pid = fork();

    // 对于子进程，先传输数据再接收数据
    if (pid == 0)
    {
        write(fds[1], str1, sizeof(str1));
        sleep(2);   // 等2s的意图何在
        // 如果不等待这2s，那么下行的read将会直接读取自己刚刚write进去的数据
        // 那么父进程当中的read将会一直等待数据读取，但是又没人写入
        // read就会无限期等待
        // 等待2s后，会让父进程中的read函数读取到子进程write进去的内容
        // 然后父进程执行write，子进程的read就可以实现成功读取
        read(fds[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    // 对于父进程，进行读取再写入
    else
    {
        read(fds[0], buf, BUF_SIZE);
        printf("Parent proc output: %s \n", buf);
        write(fds[1], str2, sizeof(str2));
        sleep(3);   // 如果不延迟3s(2、1)，父进程执行结束之后直接return 0，子进程都来不及read了
    }
    return 0;
}