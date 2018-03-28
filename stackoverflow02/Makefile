# -z execstack: 由于栈内存代码执行也是一个比较危险的漏洞，因此 gcc 在编译的时候默认开启了保护，这里我们加上这个参数关闭保护

a.out:hello.c
	gcc -g -fno-stack-protector -z execstack hello.c
clean:
	rm ./a.out
