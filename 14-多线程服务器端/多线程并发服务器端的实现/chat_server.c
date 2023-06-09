/* 
 * 从这个示例我们重点观察临界区的处理，有一个明显的特点就是：
 * 涉及到到全局变量的处理或者执行，都会用互斥量将其保护；
 * 即便不是在线程中使用这一类变量
*/
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char * argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    // 针对serv_adr又有了新的理解，由于该程序本身是做服务端使用
    // 服务端不需要主动去连接什么地址，但是客户端需要，因此客户端需要写上127.0.0.1
    // 而服务端不需要
    memset(&serv_adr, 0 ,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // 将套接字与serv_adr绑定，这个过程就证明了这是一个服务端的程序
    if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!");
    
    // 侦听有没有申请的连接
    if (listen(serv_sock, 5) == -1) error_handling("listen() error!");

    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);

        // 操作系统的内部机制(accept函数)会使得套接字获取到客户端的IP地址
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutx);      // 不是线程也需要互斥处理吗？是的，因为线程也会访问这个全局变量
        clnt_socks[clnt_cnt++] = clnt_sock; // 每当有新连接，将相关信息写入clnt_cnt和clnt_socks
        pthread_mutex_unlock(&mutx);    // 你不知道这个全局变量是线程访问还是进程访问

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id);   // 等待线程执行完毕，进程暂时阻塞
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}

void * handle_clnt(void * arg)
{
    int clnt_sock = *((int*)arg);   // 获取针对客户端的套接字
    int str_len = 0;
    char msg[BUF_SIZE];

    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
    {
        send_msg(msg, str_len);     // 该函数将消息发送到每一个客户端(包括最初发消息的那个)
    }
    
    pthread_mutex_lock(&mutx);

    for (int i = 0; i < clnt_cnt; i++)
    /* 
     * for循环是为了保证关闭客户端套接字后
     * clnt_socks数组保证一致性
    */
    {
        if (clnt_sock == clnt_socks[i])
        {
            while (i++ < clnt_cnt - 1)  clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--; // 数量减1
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);   // 安全，放心的关闭套接字
    return NULL;
}

void send_msg(char * msg, int len)
{
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
    {
        write(clnt_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mutx);
}