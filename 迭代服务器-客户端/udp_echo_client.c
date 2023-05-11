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
    char message[BUF_SIZE];
    socklen_t adr_sz;

    struct sockaddr_in serv_adr, from_adr;
    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1)  error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));   // 初始化步骤
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));
    
    while (1)
    {
        fputs("Input message(Q to quit)：", stdout);
        fgets(message, sizeof(message), stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

    // 对这个函数的理解，sock是套接字，提供了一个接口，接口内网络层会经过一些处理，获取到要发送的server的IP和端口等信息
    // 然后这个地址信息会保存在存储serv_adr的地址中，因此17行代码的定义没有初始化的必要
        sendto(sock, message, strlen(message), 0,  // 客户端将数据发送
            (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        adr_sz = sizeof(from_adr);
    // 对这个函数的理解，sock是套接字，提供了一个接口，接口内网络层会经过一些处理，获取到传输数据的server的IP和端口等信息
    // 然后这个地址信息会保存在存储from_adr的地址中。
        str_len = recvfrom(sock, message, BUF_SIZE, 0,
            (struct sockaddr*)&from_adr, &adr_sz);
        message[str_len] = 0;   // 等价于'\0'
        printf("Message from server：%s", message); // 这一步服务器会把消息传回来
    }
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);    // 在程序中调用exit(1)会立即终止当前程序,并返回状态码1。
}