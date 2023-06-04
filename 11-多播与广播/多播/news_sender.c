#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char * argv[])
{
    int send_sock;
    struct sockaddr_in mul_adr;
    int time_live = TTL;

    FILE *fp;
    char buf[BUF_SIZE];
    if(argc != 3)   // 服务端也需要提供一个多播IP是吗？
    {
        printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }

    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&mul_adr, 0, sizeof(mul_adr));
    mul_adr.sin_family = AF_INET;
    mul_adr.sin_addr.s_addr = inet_addr(argv[1]);   // 多播IP
    mul_adr.sin_port = htons(atoi(argv[2]));        // 多播端口

    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, // 设定TTL，属于sender的必要过程
                (void*) &time_live, sizeof(time_live));

    if ((fp = fopen("/home/ping/TCP-IP编程/README.md", "r")) == NULL)  error_handling("fopen() error");

    while (!feof(fp))   // 多播过程
    {
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf),
                0, (struct sockaddr*)&mul_adr, sizeof(mul_adr));
        sleep(2);   // 给传输数据提供一定的时间间隔而加的，没有什么特殊意义
    }
    fclose(fp);
    close(send_sock);
    return 0;
                
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}