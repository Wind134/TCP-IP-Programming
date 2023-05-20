/* 客户端的实现，向服务端输入几个数字，经由服务器算出相应的和，并输出
不要小看这个应用，其中精妙的体现了C语言对于字节级别的内存空间的掌控！
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024  // 设置缓冲池的最大大小，即1024字节
#define OP_SIZE 4      // 每个int型数据所占字节数
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock, result;   // sock是套接字描述符，result是服务器返回的结果
    int cnt_nums;       // 参与计算数的个数

    // 优先用char类型数组是因为方便转换，char->int从1一个字节拓展到4个字节即可，相比int还可以节省空间
    // 此外int->char的转换可能超出转换范围
    char figure_info[BUF_SIZE]; // 参与运算的数字以及运算符信息
    struct sockaddr_in serv_adr;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)  error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));   // 初始化步骤，填0

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    else puts("Connected..........");

    fputs("Operand count: ", stdout);
    scanf("%d", &cnt_nums);
    figure_info[0] = (char)cnt_nums;

    for (int i = 0; i < cnt_nums; i++)  // 输入计算的数
    {
        printf("Operand %d: ", i + 1);
        // 这里其实体现了很底层的思想，利用了char分配的连续1024字节空间的特点
        // 第一个输入为参与计算的数的数目，一般不会很多，所以用char类型
        // 此后因为进行了转换，每4个字节存储一个int，因此下标确实是要发生变化，不是常规的递增
        scanf("%d", (int *)&figure_info[i * OP_SIZE + 1]);   // 把指针转化成int型，以便用4个字节存储一个int
    }

    fgetc(stdin);   // 接换行符，起的作用是从输入流中移除该字符，缓冲之用        
    fputs("Operator: ", stdout);
    scanf("%c", &figure_info[cnt_nums * OP_SIZE + 1]);  // 输入运算符('+'或'-'或'*')
    write(sock, figure_info, cnt_nums * OP_SIZE + 2);   // 客户端将数据发送(添加了'\0'结束符，长度加1)
    read(sock, &result, OP_SIZE);     // 假设返回的结果也是一个int，长为4字节

    printf("Operation result: %d \n", result);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);    // 在程序中调用exit(1)会立即终止当前程序,并返回状态码1。
}