#include <stdio.h>      // C语言标准IO
#include <stdlib.h>     // C标准库的一个头文件，包含了一些常用函数，如动态内存分配
#include <string.h>     // 字符串相关的头文件
#include <unistd.h>     // 提供了访问POSIX操作系统API的函数
#include <arpa/inet.h>  // 该头文件提供了用于Internet地址族的一些函数
#include <sys/socket.h> // 系统头文件，实现网络和套接字编程
#include <sys/epoll.h>  // epoll功能的头文件

#define BUF_SIZE 100
#define EPOLL_SIZE 50   // events中可以保存的最大事件数
void error_handling(char *message);

/* 
struct epoll_event
{
    __uint32_t events;	// 发生的事件
    epoll_data_t data;	// data本身又是一结构体
};

typedef union epoll_data
{
    void * ptr;		// 这个是？
    int fd;			// 文件描述符
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;
 */


int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    char message[BUF_SIZE];
    int str_len, epfd, event_cnt;
    socklen_t adr_sz;

    struct epoll_event *ep_events;  // 保存诸多事件的epoll_event结构体地址
    struct epoll_event event;       // 保存诸多事件的结构体

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

    epfd = epoll_create(EPOLL_SIZE);    // 创建指定大小的epoll实例，返回epoll文件描述符
    ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);    // 分配指定事件数的空间

    event.events = EPOLLIN;     // 监视需要数据读取的情况
    event.data.fd = serv_sock;  // 要监视的套接字
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);  // 将服务器端套接字加入epoll例程

    while (1)
    {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1)
        {
            puts("epoll_wait() error");
            break;
        }

        for(int i = 0; i < event_cnt; i++)
        {
            if (ep_events[i].data.fd == serv_sock)  // 建立连接前的握手需要读取所传递的信息
            {
                adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &adr_sz);
                event.events = EPOLLIN;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);  // 加入客户端套接字
                printf("Connected client: %d \n", clnt_sock);
            }
            else    // 已经建立好了连接，这部分则是正常的信息收发
            {
                str_len = read(ep_events[i].data.fd, message, BUF_SIZE);
                if(str_len == 0)    // 如果读完了
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL); // 移除例程中的套接字
                    close(ep_events[i].data.fd);    // 关闭套接字
                    printf("Closed client: %d \n", ep_events[i].data.fd);
                }
                else
                {
                    write(ep_events[i].data.fd, message, str_len);  // 读了多少，就写回多少
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}