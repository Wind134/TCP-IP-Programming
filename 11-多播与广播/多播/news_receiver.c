#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char * argv[])
{
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;    
    struct ip_mreq join_adr;

    if(argc != 3)
    {
        printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }

    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);    // 客户端分配自个儿的IP？
    adr.sin_port = htons(atoi(argv[2]));

    if(bind(recv_sock, (struct sockaddr*) &adr, sizeof(adr)) == -1) error_handling("bind() error");

    // 从这里可以看出的一点是，接收者多做的一个步骤就是：
    // 加入多播IP，而发送者也正是将要发送的信息发送给这个多播IP
    // 而分配套接字的时候所绑定的IP也仅仅是自己的IP(INADDR_ANY)
    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]); // 加入组播IP
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);  // 加入主机套接字所属的IP

    setsockopt(recv_sock, IPPROTO_IP,
                IP_ADD_MEMBERSHIP, (void*) &join_adr, sizeof(join_adr));

    while(1)
    {
        str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if(str_len < 0) break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }            
    close(recv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}

