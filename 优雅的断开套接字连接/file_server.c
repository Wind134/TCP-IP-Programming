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
    int serv_sock, clnt_sock, read_cnt; // 服务端套接字，针对每个连接的套接字，以及读取的字节长
    char buf[BUF_SIZE];
    FILE * fp;
    struct sockaddr_in serv_adr, clnt_adr;   
    socklen_t clnt_adr_sz;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // "rb"是以二进制模式打开文件并读取文件，此模式下文件内容将被当成普通字节流
    // 不会被翻译为特定的字节流
    fp = fopen("/home/ping/代码目录/TCP-IP-Programming/优雅的断开套接字连接/send_file.txt", "rb");  

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

    while (1)
    {
        read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
        
        if (read_cnt < BUF_SIZE)  
        {
            write(clnt_sock, buf, read_cnt);
            break;
        }
        write(clnt_sock, buf, BUF_SIZE);
    }
    
    shutdown(clnt_sock, SHUT_WR);
    read(clnt_sock, buf, BUF_SIZE);
    printf("Message from client: %s \n", buf);
    fclose(fp);
    close(clnt_sock);   close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}

int calculate(int opnum, int opnds[], char op)  // 传相加数的个数，相加数组成的数组，以及运算符号
{
    int result = opnds[0], i;
    switch (op)
    {
    case '+':
        for (i = 1; i < opnum; i++) result += opnds[i];
        break;
    
    case '-':
        for (i = 1; i < opnum; i++) result -= opnds[i];
        break;
    
    case '*':
        for (i = 1; i < opnum; i++) result *= opnds[i];
        break;

    default:
        break;
    }
    return result;
}