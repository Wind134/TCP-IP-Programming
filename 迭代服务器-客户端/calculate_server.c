/* 明确服务器收到的那些信息：
要计算的数的个数、每个运算数、运算符 
不要小看这些代码，里面对C语言的运用可以说是精妙绝伦了
直接小到对每个字节的控制
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OP_SIZE 4  // 每个字符的字节大小
void error_handling(char *message);
int calculate(int, int[], char);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    char op_info_from_client[BUF_SIZE];      // 客户端源码已有注释
    int result, op_nums;    // 计算结果以及参与计算的数
    int recv_size, recv_len;    // 保存每次成功读取的字节数，以及保存服务端收到的总共字节大小
    struct sockaddr_in serv_adr, clnt_adr;   // 这两个个变量不用初始化吗？
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
    
    for (int i = 0; i < 5; i++)
    {
        op_nums = 0;    // 初始化操作数的个数，注意是整型

        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &clnt_adr_sz);
        if (clnt_sock == -1) error_handling("accept() error!");
        else printf("Connected client %d \n", i + 1);

        // 这一步读取1个字节的op_nums后，会将char类型变为int型，很完美(C技巧)
        read(clnt_sock, &op_nums, 1);   // 读取了操作数的个数，那我们就知道了剩余的字节数为op_nums * OP_SIZE + 1
        
        recv_len = 0;
        while (recv_len < (op_nums * OP_SIZE + 1))  // 该数组包含操作符一共op_nums * OP_SIZE + 1字节
        {
            recv_size = read(clnt_sock, &op_info_from_client[recv_len], BUF_SIZE - 1);
            recv_len += recv_size;
        }
        
        // 下面又是一次转换，将char->int型
        result = calculate(op_nums, (int*)op_info_from_client, op_info_from_client[recv_len - 1]);
        write(clnt_sock, &result, OP_SIZE);
        close(clnt_sock);
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