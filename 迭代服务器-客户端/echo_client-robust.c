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
    int sock, str_len, read_len, recv_len;
    char message[BUF_SIZE];
    struct sockaddr_in serv_adr;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)  error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));   // 初始化步骤
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    else puts("Connected..........");    
    
    while (1)
    {
        fputs("Input message(Q to quit)：", stdout);
        fgets(message, BUF_SIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        str_len = write(sock, message, strlen(message));  // 客户端将数据发送，保存写入传输的数据长度

        recv_len = 0;
        while (recv_len < str_len)
        {
            read_len = read(sock, message, BUF_SIZE - 1);

            if (read_len == -1) error_handling("read() error!");
            
            recv_len += read_len;
        }
        
        message[str_len] = 0;   // 等价于'\0'
        printf("Message from server: %s", message); // 这一步服务器会把消息传回来
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