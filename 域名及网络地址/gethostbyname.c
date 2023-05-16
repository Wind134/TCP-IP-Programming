#include <stdio.h>  // C语言标准库IO
#include <stdlib.h> // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <arpa/inet.h>  // 该头文件提供了用于Internet地址族的一些函数
#include <netdb.h>      // 该头文件用于进行网络相关的操作和访问网络数据库。
                        // 提供了与主机名解析、服务名解析、网络地址转换等相关的函数和数据结构

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int i;
    struct hostent *host;
    if (argc != 2)
    {
        printf("Usage : %s <addr>\n", argv[0]);
        exit(1);
    }
    
    host = gethostbyname(argv[1]);
    if(!host)   error_handling("gethost... error");

    printf("Offical name: %s \n", host->h_name);
    for (i = 0; host->h_aliases[i]; i++)    printf("Aliases %d: %s \n", i + 1, host->h_aliases[i]);

    printf("Address type: %s \n", (host->h_addrtype == AF_INET)?"AF_INET":"AF_INET6");

    for (i = 0; host->h_addr_list[i]; i++)
        printf("IP addr %d: %s \n", i + 1, 
            inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);                // 出现错误，则终止整个程序
}