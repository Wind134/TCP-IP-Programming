#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int cfd1, cfd2;
    char str1[] = "Hi~ \n";
    char str2[] = "It's nice day~ \n";

    cfd1 = dup(1);  // 复制文件描述符1(标准输入还是标准输出来着)
    cfd2 = dup2(cfd1, 7);   // 再复制文件描述符，指定复制出的文件描述符为7

    printf("fd1 = %d, fd2 = %d \n", cfd1, cfd2);
    write(cfd1, str1, sizeof(str1));
    write(cfd2, str2, sizeof(str2));

    close(cfd1);    // 关闭复制出来的cfd1
    close(cfd2);
    write(1, str1, sizeof(str1));
    close(1);
    write(1, str2, sizeof(str2));   // 这一行不会输出，因为上面已经将之关闭了
    return 0;
}