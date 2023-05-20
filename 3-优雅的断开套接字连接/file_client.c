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
    int sock, read_cnt; // 本客户端套接字，以及读取的字节长
    char buf[BUF_SIZE];
    FILE * fp;
    struct sockaddr_in serv_adr;   

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // "rb"是以二进制模式打开文件并读取文件，此模式下文件内容将被当成普通字节流
    // 不会被翻译为特定的字节流
    fp = fopen("send_file.txt", "wb");  

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)  error_handling("socket() error");
    memset(&serv_adr, 0, sizeof(serv_adr)); // 填充0，初始化(不初始化会怎样？)
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    else puts("Connected..........");

    while ((read_cnt = read(sock, buf, BUF_SIZE)) != 0)
        fwrite((void*)buf, 1, read_cnt, fp);
    
    puts("Received file data");
    
    write(sock, "Thank you", 10);   // 这是客户端发送的结尾的信息

    fclose(fp);
    close(sock);
    
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