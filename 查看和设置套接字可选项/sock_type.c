#include <stdio.h>  // C语言标准库IO
#include <stdlib.h> // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <sys/socket.h>  // 提供与socket相关的数据结构


void error_handling(char *message);

int main(int argc, char *argv[])
{
    int tcp_sock, udp_sock, sock_type;
    socklen_t optlen;
    int state;
    
    optlen = sizeof(sock_type);
    
    tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    udp_sock = socket(PF_INET, SOCK_DGRAM, 0);

    printf("SOCK_STREAM: %d \n", SOCK_STREAM);
    printf("SOCK_DGRAM: %d \n", SOCK_DGRAM);

    state= getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
    if(state)   error_handling("getsockopt() error");
    printf("Socket type one: %d \n", sock_type);    // 套接字类型只能在创建时决定，以后不能再更改

    state= getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
    if(state)   error_handling("getsockopt() error");
    printf("Socket type two: %d \n", sock_type);
    
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);                // 出现错误，则终止整个程序
}