/* 这是一个服务端角度 */
#include <stdio.h>      // C语言标准IO
#include <stdlib.h>     // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <string.h>     // 字符串相关的头文件
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <arpa/inet.h>  // 该头文件提供了用于Internet地址族的一些函数
#include <sys/socket.h> // 系统头文件，实现网络和套接字编程

#define BUF_SIZE 30
void error_handling(char *message);

int acpt_sock;
int recv_sock;

int main(int argc, char* argv[])
{
    int acpt_sock, recv_sock;
    char buf[BUF_SIZE];
    struct sockaddr_in acpt_adr, recv_adr;
    socklen_t recv_adr_sz;
    int state, str_len;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);    // 生成套接字
    if(acpt_sock == -1)  error_handling("socket() error");
    memset(&acpt_adr, 0, sizeof(acpt_adr)); // 这是接收数据的地址(也就是服务器自己的地址)
    acpt_adr.sin_family = AF_INET;
    acpt_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    acpt_adr.sin_port = htons(atoi(argv[1]));

    // 下面的bind函数分配IP地址
    if (bind(acpt_sock, (struct sockaddr*) &acpt_adr, sizeof(acpt_adr)) == -1)
        error_handling("bind() error!");
    
    if (listen(acpt_sock, 5) == -1) error_handling("listen() error!");

    recv_adr_sz = sizeof(recv_adr);
    recv_sock = accept(acpt_sock, (struct sockaddr*)&recv_adr, &recv_adr_sz);
    


    // // 信号处理函数处理SIGURG信号，如果接收到信号，就通过act的地址获取处理该信号的函数，进行处理
    // state = sigaction(SIGURG, &act, 0);

    while(1)
    {
        // 调用recv函数的同时传递MSG_DONTWAIT可选项，保证即使不存在待读取数据也不会进入阻塞状态
        str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_PEEK | MSG_DONTWAIT);
        if(str_len > 0) break;
    }

    buf[str_len] = 0;
    printf("Buffering %d bytes: %s \n", str_len, buf);

    // 再次调用recv函数，未设置任何可选项，因此本次读取的数据将从输入缓冲中删除
    str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_PEEK | MSG_DONTWAIT);
    buf[str_len] = 0;
    printf("Read again: %s \n", buf);
    close(acpt_sock);
    close(recv_sock);
    return 0;
}


void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}