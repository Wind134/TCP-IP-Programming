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
    int serv_sock, clnt_sock;
    char buf[BUF_SIZE] = {0, }; // 是一个用于声明字符数组的语句，并将数组中的所有元素初始化为0。
    FILE * readfp;
    FILE * writefp;

    struct sockaddr_in serv_adr, clnt_adr;   
    socklen_t clnt_adr_sz;

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

    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz);
    if (clnt_sock == -1) error_handling("accept() error!");

    
    readfp = fdopen(clnt_sock, "r");
    writefp = fdopen(dup(clnt_sock), "w");

    fputs("FROM SERVER: Hi~ client? \n", writefp);        
    fputs("I love all of the world \n", writefp);       
    fputs("You are awesome! \n", writefp);       
    fflush(writefp);
    
    shutdown(fileno(writefp), SHUT_WR); // writefp->套接字描述符
    fclose(writefp);                // fclose函数终止了dup出来的套接字writefp，但是clnt_sock套接字还在
    fgets(buf, sizeof(buf), readfp);// 因此会接收到客户端发来的Thank You
    fputs(buf, stdout);

    fclose(readfp);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}