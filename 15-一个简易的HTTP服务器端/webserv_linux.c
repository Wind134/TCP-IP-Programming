#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void* request_handler(void* arg);
void send_data(FILE * fp, char * ct, char* file_name);
char* content_type(char* file);
void send_error(FILE* fp);
void error_handling(char* message);

int main(int argc, char * argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_size;
    char buf[BUF_SIZE];
    pthread_t t_id;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

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
        clnt_adr_size = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
        printf("Connection Request : %s:%d\n",
                inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
        pthread_create(&t_id, NULL, request_handler, &clnt_sock);
        pthread_detach(t_id);        
    }
    close(serv_sock);
    return 0;
}

void* request_handler(void *arg)
{
    int clnt_sock = *((int*)arg);
    char req_line[SMALL_BUF];
    FILE* clnt_read;
    FILE* clnt_write;

    char method[10];
    char ct[15];
    char file_name[30];

    clnt_read = fdopen(clnt_sock, "r");
    clnt_write = fdopen(dup(clnt_sock), "w");
    fgets(req_line, SMALL_BUF, clnt_read);
    if(strstr(req_line, "HTTP/") == NULL)   // 在一个较长的字符串中查找子串的第一次出现位置
    /*
     * 如果没有"HTTP/"字符，那么直接关闭连接，并且报错 
    */
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }
    strcpy(method, strtok(req_line, " /"));
    
    // 在后续的调用中，如果传入的str为NULL，则函数会继续
    // 在上一次调用的剩余部分中查找下一个分隔符之前的子串。
    strcpy(file_name, strtok(NULL, " /"));  // 这一行可能有问题，尝试去掉'/'
    strcpy(ct, content_type(file_name));

    if (strcmp(method, "GET") != 0)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    fclose(clnt_read);
    send_data(clnt_write, ct, file_name);
    
}

void send_data(FILE* fp, char* ct, char* file_name)
{
    char protocol[] = "HTTP/1.1 200 ok\r\n";
    char server[] = "Server: Linux Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[SMALL_BUF];
    char buf[BUF_SIZE];
    FILE* send_file;

    sprintf(cnt_type, "Content-type: %s\r\n\r\n", ct);
    send_file = fopen(file_name, "r");
    if(send_file == NULL)
    {
        send_error(fp);
        return;
    }

    /* 传输头信息 */
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);

    /* 传输请求数据 */
    while(fgets(buf, BUF_SIZE, send_file) != NULL)
    {
        fputs(buf, fp);
        fflush(fp);
    }
    fflush(fp);
    fclose(fp);
}

char* content_type(char* file)
{
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);
    strtok(file_name, ".");
    strcpy(extension, strtok(NULL, "."));
    if(!strcmp(extension, "html") || !strcmp(extension, "htm"))
        return  "text/html";
    else
        return "text/plain";
}

void send_error(FILE* fp)   /* 报错信息写入 */
{
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Server: Linux Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[] = "Content-type:text/html\r\n\r\n";

    // 在C语言中，字符串常量可以由多个字符串字面值拼接而成，它们会在编译时自动连接起来形成一个连续的字符串。
    char content[] = "<html><head><title>NETWORK</title></head>"
                    "<body><font size=+5><br>发生错误！查看请求文件名和请求方式！"
                    "</font></body></html>";
    fputs(protocol, fp);                
    fputs(server, fp);                
    fputs(cnt_len, fp);                
    fputs(cnt_type, fp);
    fflush(fp);                
}

void error_handling(char* message)
{
    fputs(message, stderr); // stderr是类似printf的一种输出流，只是专用于错误数据输出
    fputc('\n', stderr);
    exit(1);
}