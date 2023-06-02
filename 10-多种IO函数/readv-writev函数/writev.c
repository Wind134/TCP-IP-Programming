#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char *argv[])
{
    struct iovec vec[2];    // 大小为2的数组，数组成员类型为struct iovec
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567";
    int str_len;

    vec[0].iov_base = buf1;
    vec[0].iov_len = 3;
    vec[1].iov_base = buf2;
    vec[1].iov_len = 4;

    str_len = writev(1, vec, 2);    // 代码从这里写入到标准输出1，因此我们可以在终端看到内容
    puts("");
    printf("Write bytes: %d \n", str_len);
    return 0;
}