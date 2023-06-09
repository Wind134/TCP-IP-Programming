#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void * recv_msg(void * arg);
void * send_msg(void * arg);
void error_handling(char * msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char * argv[])
{
    int sock;
    struct sockaddr_in serv_adr; // 先定义要连接的服务端地址
    pthread_t snd_thread, rcv_thread;

    void * thread_return;

    if (argc != 4)
    {
        printf("Usage: %s <IP> <Port> <Name> \n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0); // 客户端分配套接字

    memset(&serv_adr, 0 ,sizeof(serv_adr));   // 处理客户端的IP
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);  // 线程1处理信息的发送
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);  // 线程2处理信息的接收
    pthread_join(snd_thread, &thread_return);   // 等待线程1执行完毕
    pthread_join(rcv_thread, &thread_return);   // 等待线程2执行完毕

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}

void * send_msg(void * arg)
{
    int sock = *((int*) arg);   // 由进程传递而来的参数变量
    char name_msg[NAME_SIZE + BUF_SIZE];    // name_msg被设定为名字+信息的组合
    while (1)
    {
        fgets(msg, BUF_SIZE, stdin);    // 输入字符q则结束
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s %s", name, msg);  // 将name、msg都存入->name_msg
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}

void * recv_msg(void * arg)
{
    int sock = *((int*) arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    while (1)
    {
        str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        if(str_len == -1)   return (void*)-1;
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);    // 输出信息
    }
    return NULL;
}