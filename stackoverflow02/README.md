#### 目标
1. 首先我们要获取执行 /bin/sh 的汇编机器码
2. 其次我们要获取栈上 main 的返回地址和 buffer 之间的偏移
3. 最后就是获取通过 printf 打印出来的首地址
4. 构造 payload 为：payload = 汇编机器码 + 无用字符 * (偏移 - len(汇编机器码)) + buffer 首地址

#### 调试过程

###### shellcode 获取
这里直接使用 pwntools 提供的 asm 函数直接将汇编指令转为机器码。

###### 计算偏移
```
gdb ./a.out
start
p buffer
p &buffer  # 0x7fffffffe3d0(不一定相同)
info f  # 0x7fffffffe428(不一定相同)
quit
```

###### 安装依赖包
```
sudo pip intall pwn
sudo pip install binascii
```
