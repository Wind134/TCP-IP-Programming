#include <stdio.h>  // C语言标准库IO
#include <stdlib.h> // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <sys/socket.h>  // 提供与socket相关的数据结构

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    int snd_buf = 1024 * 2, rcv_buf=1024 * 2, state;
    socklen_t len;

    sock = socket(PF_INET, SOCK_STREAM, 0);

    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*) &snd_buf, sizeof(snd_buf));
    if(state)   error_handling("setsockopt() error");

    state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*) &rcv_buf, sizeof(rcv_buf));
    if(state)   error_handling("setsockopt() error");

    len = sizeof(snd_buf);      // 输出缓冲区的大小
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*) &snd_buf, &len);
    if(state)   error_handling("getsockopt() error");

    len = sizeof(rcv_buf);      // 输入缓冲区的大小
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*) &rcv_buf, &len);
    if(state)   error_handling("getsockopt() error");

    /*
    * 一个比较有趣的事实是，下面两行输出的内容并不会与我们的预期完全一致
    * TCP本身需要实现流控制和错误发生时的重传机制，至少会需要一些缓冲空间，这部分
    * 套接字内部会处理好这一类细节信息；
    */
    printf("Input buffer size: %d \n", rcv_buf);
    printf("Output buffer size: %d \n", snd_buf);
    return 0;
}


void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);                // 出现错误，则终止整个程序
}