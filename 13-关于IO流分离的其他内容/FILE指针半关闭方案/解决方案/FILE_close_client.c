#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock;
    char buf[BUF_SIZE];
    FILE * readfp;
    FILE * writefp;
    struct sockaddr_in serv_adr;   

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)  error_handling("socket() error");
    memset(&serv_adr, 0, sizeof(serv_adr)); // 填充0，初始化(不初始化会怎样？)
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    else puts("Connected..........");

    readfp = fdopen(sock, "r");
    writefp = fdopen(sock, "w");

    while(1)
    {
        if (fgets(buf, sizeof(buf), readfp) == NULL)    break;
        fputs(buf, stdout);
        fflush(stdout);
    }
    
    fputs("FROM CLIENT: Thank you! \n", writefp);
    fflush(writefp);

    fclose(writefp);
    fclose(readfp);
    close(sock);
    
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}