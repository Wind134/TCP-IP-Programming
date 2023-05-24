#include <stdio.h>      // C语言标准IO
#include <stdlib.h>     // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <string.h>     // 字符串相关的头文件
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <signal.h>     // 信号处理机制
#include <sys/wait.h>   // 回收子进程
#include <arpa/inet.h>  // 该头文件提供了用于Internet地址族的一些函数
#include <sys/socket.h> // 系统头文件，实现网络和套接字编程

#define BUF_SIZE 100
void error_handling(char *message);
void read_childproc(int);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;

    // 管道使用的文件描述符定义
    int fds[2];
    
    pid_t pid;
    int state, str_len;     // 定义子进程ID，read读取的str长度
    struct sigaction act;   // 定义sigaction要用的结构体变量
    socklen_t adr_sz;       // 客户端地址长
    char buf[BUF_SIZE];     // 缓冲区

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 下面这部分是用来管理子进程的
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    // 下面这部分是用来分配套接字的一系列操作
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);    // 也是所有进程的公共部分
    if(serv_sock == -1)  error_handling("socket() error");

    // 下面这部分是分配IP地址以及相关地址族的一系列操作
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // 采用INADDR_ANY可以自动获取运行服务器端的计算机IP地址
    serv_adr.sin_port = htons(atoi(argv[1]));

    // binding
    if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");
    // listen
    if (listen(serv_sock, 5) == -1) error_handling("listen() error!");
    
    // 下面这部分是管道的处理，以及文件的写入
    pipe(fds);
    pid = fork();
    if(pid == 0)    // 子进程的任务
    {
        FILE *fp = fopen("/home/ping/echomsg.txt", "wt");    // 打开并截断式写入？
        char msgbuf[BUF_SIZE];  // 定义管道能处理的数据量？
        int len;

        for (int i = 0; i < 10; i++)
        {
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void*)msgbuf, 1, len, fp);  // 写入
        }

        fclose(fp);

        sleep(10);
        return 0;
        
    }
    
    // 这部分是父进程分配子进程的一系列操作
    while (1)
    {
        adr_sz = sizeof(clnt_adr);
        // 如果一直没有客户端的连接请求到达，accept会处于阻塞态
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &adr_sz);   // 公共部分
        if (clnt_sock == -1)
        {
            continue;   // accept执行失败，则继续一个新的循环
        }
        else puts("new client connected...");   // 成功，输出成功建立连接的信息

        pid = fork();   // fork出子进程去执行任务
        if (pid == -1)
        {
            close(clnt_sock);   // fork失败，那就去执行新的
            continue;
        }

        if (pid == 0)   // 子进程执行的部分
        {
            // 子进程会获取到同样的套接字描述符，关闭这部分，否则套接字无法被完全关闭
            // 因为serv_sock实质上只是一个套接字文件描述符，只要有文件描述符存在
            // 那套接字就无法完全销毁
            close(serv_sock);   
            // 这部分读取和写入都在子进程部分进行，后续可以考虑进行分割
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
            {
                write(clnt_sock, buf, str_len); // 将读取到的信息写回
                write(fds[1], buf, str_len);
            }

            close(clnt_sock);
            puts("client disconnected...");
            sleep(10);
            return 0;   // 子进程执行完毕就直接返回    
        }
        else
        {
            close(clnt_sock);   // 父进程啥也不干，就负责建立连接以及管理子进程以及销毁连接的客户端套接字
        }        
    }    
    
    close(serv_sock);
    return 0;
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}