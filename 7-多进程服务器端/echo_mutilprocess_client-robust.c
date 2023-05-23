#include <stdio.h>      // C语言标准IO
#include <stdlib.h>     // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <string.h>     // 字符串相关的头文件
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <arpa/inet.h>  // 该头文件提供了用于Internet地址族的一些函数
#include <sys/socket.h> // 系统头文件，实现网络和套接字编程

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int, char*);
void write_routine(int, char*);

int main(int argc, char* argv[])
{
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);    // exit(1)是一个终止程序的函数调用，它会立即停止程序的执行，并返回给操作系统一个退出码
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)  error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));   // 初始化步骤
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    else puts("Connected..........");    
    
    pid = fork();
    if (pid == 0)   write_routine(sock, buf);   // 子进程负责写入
    else    read_routine(sock, buf);            // 父进程负责读取
    
    close(sock);
    return 0;
}

// 这个读取函数保证了对边界的处理
void read_routine(int sock, char *buf)
{
    while (1)
    {
        int str_len = read(sock, buf, BUF_SIZE);
        if (str_len == 0)   return;

        buf[str_len] = 0;
        printf("Message from server: %s", buf);
    }
}

void write_routine(int sock, char *buf)
{
    while (1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
        {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);    // 在程序中调用exit(1)会立即终止当前程序,并返回状态码1。
}