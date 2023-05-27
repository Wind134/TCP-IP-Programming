#include <stdio.h>      // C语言标准IO
#include <stdlib.h>     // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <string.h>     // 字符串相关的头文件
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <arpa/inet.h>  // 该头文件提供了用于Internet地址族的一些函数
#include <sys/socket.h> // 系统头文件，实现网络和套接字编程
#include <sys/time.h>   // 设置时间相关的功能头文件
#include <sys/select.h> // 系统头文件，包含select函数的头文件

#define BUF_SIZE 100
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    char message[BUF_SIZE];
    char buf[BUF_SIZE];
    fd_set reads, cpy_reads;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;
    socklen_t adr_sz;
    int fd_max, str_len, fd_num;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)  error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");
    
    if (listen(serv_sock, 5) == -1) error_handling("listen() error!");

    FD_ZERO(&reads);
    
    // 设置了对serv_sock套接字的监视
    // 同时客户端的连接请求同样通过传输数据完成(连接请求的套接字与数据传输的套接字不是同一个)
    // 因此服务器套接字中有接收的数据，就意味着有新的连接请求
    FD_SET(serv_sock, &reads);
    FD_SET(0, &reads);           // 对标准输入进行监视
    fd_max = serv_sock;          // 最大监视范围

    while(1)
    {
        cpy_reads = reads;      // 每次循环都将状态重置为初始状态
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        // select执行失败，则退出循环
        if((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1) break;

        if(fd_num == 0) continue;   // 如果等待，则继续

        for (int i = 0; i < fd_max + 1; i++)
        {
            if(FD_ISSET(i, &cpy_reads))    // 查找每一个状态发生变化的套接字描述符
            {
                if (i == 0)     // 如果有输入，程序将其输出
                {
                    str_len = read(0, buf, BUF_SIZE);
                    if (str_len == 0)   buf[BUF_SIZE] = 0;
                    printf("\nMessage from console: %s", buf);
                    if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))  error_handling("Normal quit");
                }
                if(i == serv_sock)  // 如果这个发生变化的套接字是服务器套接字，说明这时发生了连接请求
                {
                    adr_sz = sizeof(clnt_adr);

                    // clnt_sock是服务器中管理客户端连接的套接字
                    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &adr_sz);
                    if (clnt_sock == -1) error_handling("accept() error!");
                    
                    // 下面这部分注册了服务器中与客户端连接的套接字文件描述符
                    // 因为后续数据传输通过这个套接字进行
                    FD_SET(clnt_sock, &reads);

                    // 如果分配的套接字超过了之前的最大的描述文件符号，则更新fd_max
                    if(fd_max < clnt_sock)  fd_max = clnt_sock;
                    printf("Connected client %d \n", clnt_sock);
                }
                else    // 如果发生变化的不是分配的服务器套接字，那就应该是管理连接的套接字，则进行数据读取
                {
                    str_len = read(i, message, BUF_SIZE);

                    // 下面会不会有边界问题，我们怎么确保一次可以读取客户端传来的所有数据？
                    // 通过调整缓冲区大小BUF_SIZE证实了想法是正确的
                    // while ((str_len = read(i, message, BUF_SIZE)) != 0)
                    // {
                    //     write(i, message, str_len);
                    // }

                    if(str_len == 0)
                    {
                        FD_CLR(i, &reads);
                        close(i);   // 只要没关闭连接，while循环保证后面还是会读取到该客户端
                        printf("closed client: %d \n", i);
                    }
                    else
                    {
                        write(i, message, str_len); // 数据写回，实现echo
                    }
                }
            }
        }
    }
    
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}