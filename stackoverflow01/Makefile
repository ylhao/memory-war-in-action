# -g: 让可执行程序可以被 gdb 调试器调试，我们需要用 gdb 来计算 buffer 的首地址到 main 函数的返回地址的偏移，从而才可以知道我们的 payload 的长度
# -fno-stack-protector: 关闭 gcc 的栈保护机制，由于栈溢出是一个非常危险的漏洞，因此新版本的 gcc 在编译程序的时候是默认开启了栈保护的

a.out:hello.c
	gcc -g -fno-stack-protector hello.c
clean:
	rm ./a.out
