#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

// 栈溢出后执行的函数
void bingo() {
    // 调用 system 函数启动 /bin/sh 来获取 shell
    system("/bin/sh");
    puts("Hacked By Sniper.");
}

int main() {
    int a = 0;
    char buffer[36] = {0};
    puts("Tell me why : ");
    /*
     * 溢出的原因就是因为 read 函数没有对 buffer 数组的范围进行检查
     * 如果我们向标准输入流中输入超过 buffer 范围（36个字节）的数据，那么写操作也不会停止，而是会继续向内存中写入数据，而写入什么数据是由我们控制的。
     * buffer 数组是保存在内存中的栈段中的，而 main 函数的返回地址也是保存在栈段中的。
     * 因此我们只需要控制写入的数据将 main 函数的返回地址覆盖，这样在主函数执行结束后，会 pop 栈中保存的主函数的返回地址（而事实上，主函数的返回地址已经被我们覆盖）到 eip 寄存器中。
     * 这个时候 CPU 就不会认为程序已经结束，而是继续根据 eip 寄存器指向的内存取指令执行，这样我们就可以达到任意控制程序执行流程的目的。
     * 因此为了获取一个 shell，我们需要将主函数的返回地址覆盖为 bingo 的函数地址，这样程序执行遇到 return 0 就会直接跳到 bingo 函数，从而执行 system("/bin/sh") 获取一个 shell。
     * 远程操作和本机演示并无本质上的区别。
     * */
    read(0, buffer, 0xFF);
    printf("Good boy : %s\n", buffer);
    return 0;
}
