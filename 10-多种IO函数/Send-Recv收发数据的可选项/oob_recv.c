/* 这是一个服务端角度 */
#include <stdio.h>      // C语言标准IO
#include <stdlib.h>     // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <string.h>     // 字符串相关的头文件
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <arpa/inet.h>  // 该头文件提供了用于Internet地址族的一些函数
#include <sys/socket.h> // 系统头文件，实现网络和套接字编程
#include <signal.h>     // 信号处理机制
#include <fcntl.h>      // 通常与signal结合使用的头文件

#define BUF_SIZE 30
void error_handling(char *message);
void urg_handler(int signo);    // 紧急处理函数，用于处理MSG_OOB紧急消息

int acpt_sock;
int recv_sock;

int main(int argc, char* argv[])
{
    struct sigaction act;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr, recv_adr;
    socklen_t serv_adr_sz;
    int state, str_len;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    act.sa_handler = urg_handler;   // // sa_handler成员保存信号处理函数的指针值
    sigemptyset(&act.sa_mask);  // 以下两个参数都暂时初始化为0，具体的用法后续补充
    act.sa_flags = 0;

    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);    // 生成套接字
    if(acpt_sock == -1)  error_handling("socket() error");
    memset(&recv_adr, 0, sizeof(recv_adr)); // 这是接收数据的地址(也就是服务器自己的地址)
    recv_adr.sin_family = AF_INET;
    recv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_adr.sin_port = htons(atoi(argv[1]));

    // 下面的bind函数分配IP地址
    if (bind(acpt_sock, (struct sockaddr*) &recv_adr, sizeof(recv_adr)) == -1)
        error_handling("bind() error!");
    
    if (listen(acpt_sock, 5) == -1) error_handling("listen() error!");

    serv_adr_sz = sizeof(serv_adr);
    recv_sock = accept(acpt_sock, (struct sockaddr*)&serv_adr, &serv_adr_sz);
    
    // fcntl函数用于控制文件描述符，下面该行的意义是：
    // 将recv_sock描述符的拥有者(F_SETOWN)改为把getrid函数返回值用作ID的进程
    // getrid()返回是调用此函数的进程，也就是说，让合适的进程去进行信号处理，而
    // 不是任何进程都可以处理SIGURG信号
    fcntl(recv_sock, F_SETOWN, getpid());

    // 信号处理函数处理SIGURG信号，如果接收到信号，就通过act的地址获取处理该信号的函数，进行处理
    state = sigaction(SIGURG, &act, 0);

    while((str_len = recv(recv_sock, buf, sizeof(buf), 0)) != 0)
    {
        if(str_len == -1)   continue;   // 接收失败则重新接收
        buf[str_len] = 0;
        puts(buf);
    }
    close(recv_sock);
    close(acpt_sock);
    return 0;
}

void urg_handler(int signo)
{
    int str_len;
    char buf[BUF_SIZE];
    str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_OOB);
    buf[str_len] = 0;
    printf("Urgent message: %s \n", buf);
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}