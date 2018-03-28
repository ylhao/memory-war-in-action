#include<stdio.h>
#include<unistd.h>

/*
 * 这次程序没有给我们像 stackoverflow01 那样可以调用的 /bin/sh 的 bingo 函数
 * 我们的思路是自己用汇编语言写一个调用 /bin/sh 的程序，将这个程序汇编以后得到的机器码（注意必须是相同平台，否则机器码不会正常执行）通过 read 函数写入到内存中
 * read 函数不会检查 buffer 数组的边界，因此我们可以覆盖掉 main 函数的返回地址
 * 我们先将机器码写入 buffer， 然后再填充一些无用字符直到 main 函数的返回地址
 * 然后我们用机器码的地址覆盖 main 的返回地址
 * 以上就需要我们获取 buffer 的首地址，计算机器码的偏移（如果从 buffer 首地址就开始写机器码，那偏移就是 0， 这时机器码首地址就是 buffer 数组的首地址），然后得到机器码的首地址，之后用这个地址覆盖 main 的返回地址
 * 至此工作完成
 * */
int main() {
    // 定义一个 80 个字节的字符数组
    char buffer[0x50] = {0};
    // 打印字符数组在内存（栈段）中的地址
    printf("&buffer = %p\n", &buffer);
    // 刷新缓冲区
    fflush(stdout);
    // 将缓冲区中的数据读入到 buffer 数组
    read(0, buffer, 0xFF);
    // 打印 buffer 数组
    printf("Content of buffer : %s\n", buffer);
    return 0;
}
