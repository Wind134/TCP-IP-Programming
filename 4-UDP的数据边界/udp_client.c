#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock, str_len;
    char msg1[] = "Hi!";
    char msg2[] = "I'm another UDP host!";
    char msg3[] = "Nice to meet you";

    socklen_t adr_sz;
    struct sockaddr_in my_adr;
    
    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)  error_handling("socket() error");

    memset(&my_adr, 0, sizeof(my_adr));   // 初始化步骤
    my_adr.sin_family = AF_INET;
    my_adr.sin_addr.s_addr = inet_addr(argv[1]);
    my_adr.sin_port = htons(atoi(argv[2]));
    

    sendto(sock, msg1, strlen(msg1), 0,  // 客户端将数据发送，同时获取地址
            (struct sockaddr*)&my_adr, sizeof(my_adr));
    sendto(sock, msg2, strlen(msg2), 0,  // 客户端将数据发送，同时获取地址
            (struct sockaddr*)&my_adr, sizeof(my_adr));
    sendto(sock, msg3, strlen(msg3), 0,  // 客户端将数据发送，同时获取地址
            (struct sockaddr*)&my_adr, sizeof(my_adr));
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);    // 在程序中调用exit(1)会立即终止当前程序,并返回状态码1。
}