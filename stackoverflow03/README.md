# 函数调用过程学习
函数调用是一个最简单不过的概念了，然而每次发生函数调用，CPU 和操作系统内核都做了大量的工作。这篇文章只分析到 compute() 函数执行。
[参考](https://blog.csdn.net/wenqiang1208/article/details/74353303)
### caller 和 callee
首先说明一个概念，下面的代码片段中，我们说 main 函数是主调函数（caller），compute 函数则是被调函数（callee）。
``` c
int main(void)
{
    compute(2, 4); 
    return 0;
}    
```
### 准备
``` c
// compute.c
// 加法操作
int add(int c, int d)
{
    int e = c + d;
    return e;
}
 
// 运算
int compute(int a, int b)
{
    int e = add(a, b);
    return e;
}
 
// 主函数
int main(void)
{
    compute(2, 4);
    return 0;
}         
```
将以上这个 C 程序编译得到可执行文件，当这个可执行文件运行时，在操作系统中对应一个进程，这个进程在用户态对应一个调用栈结构(call stack)。程序中每一个**未完成运行的函数**对应一个栈帧(stack frame)，栈帧可以理解成一个函数在栈上对应的一段连续空间。栈帧中保存函数局部变量、传递给被调函数的参数等信息。 **当前帧**的范围是由两个寄存器来界定的。这两个寄存器分别位 **BP(Base Pointer)** 寄存器和 **SP(Stack Pointer)** 寄存器。BP 寄存器也叫基址寄存器。SP 寄存器也叫栈顶寄存器。另外栈底对应高地址，栈顶对应低地址，栈由内存高地址向低地址生长。上面所说的是一些概念，先知道这些概念，然后大概清楚程序执行过程中，随着函数调用的发生，该进程对应的栈结构大致如下。
![](http://upload-images.jianshu.io/upload_images/9966001-38914b8470edd68c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)


### 寄存器
寄存器位于CPU内部，用于存放程序执行中用到的数据和指令，CPU从寄存器中取数据，相比从内存中取快得多。寄存器又分**通用寄存器**和**特殊寄存器**。
通用寄存器有 **ax/bx/cx/dx/di/si**，尽管这些寄存器在大多数指令中可以任意选用，但也有一些特殊的规定，比如某些指令只能用某个特定的**通用寄存器**，例如函数返回时，需将返回值 mov 到 ax 寄存器中，特殊寄存器有 **bp/sp/ip** 等，特殊寄存器均有特定用途，对于有特定用途的几个寄存器，简要介绍如下：
ax(accumulator): 可用于存放函数返回值
bp(base pointer): 用于存放**执行中的函数**对应的栈帧的栈底地址
sp(stack poinger): 用于存放**执行中的函数**对应的栈帧的栈顶地址
ip(instruction pointer): 指向当前执行指令的下一条指令
另外不同架构的 CPU，寄存器名称被添以不同前缀以指示寄存器的大小。例如对于 x86 架构，字母 “e” 用作名称前缀，指示各寄存器大小为 32 位，对于 x86_64 寄存器，字母 “r” 用作名称前缀，指示各寄存器大小为 64 位。

### 编译
首先我们编译上面的程序，如果想用 gdb 调试工具进行调试，这里必须加上 -g 参数，加上 -g 参数后，目标文件中才能包含调试要用到的信息。
``` shell
gcc -g compute.c -o compute
```

### 反汇编分析
输入以下命令，进入调试环境。
``` shell
gdb compute
```
进入调试环境以后，输入`start`命令开始调试。`start` 命令用于拉起被调试程序，并执行至 main 函数的开始位置，程序被执行之后与一个用户态的调用栈关联。
```
start
```
主要的输出信息如下：
```
Temporary breakpoint 1, main () at compute.c:26
26	    compute(2, 4);
```
现在我们的程序跑在`main`函数中，并在第 26 行处，也就是 compute(2, 4) 这个位置停住了（该行的代码还没执行）。我们`disassemble`命令显示当前函数的汇编信息。我们用到了`-r`参数和`-m`参数。`-m`参数是指定显示的计算机指令用16进制表示。`/m`参数指定显示汇编指令的同时，显示相应的源代码。
```
disassemble /rm
```
显示的主要结果如下，其中 # 后面的内容为人为添加注释。
```
Dump of assembler code for function main:
25	{  # 源文件行号，该行代码
   0x000000000040055b <+0>:	55	push   %rbp
   0x000000000040055c <+1>:	48 89 e5	mov    %rsp,%rbp

26	    compute(2, 4);
# 注意下面这个箭头，表明程序现在停在这个地方，该行代码还没有执行。
=> 0x000000000040055f <+4>:	be 04 00 00 00	mov    $0x4,%esi
   0x0000000000400564 <+9>:	bf 02 00 00 00	mov    $0x2,%edi
   0x0000000000400569 <+14>:	e8 b2 ff ff ff	callq  0x400520 <compute>

27	    return 0;
   0x000000000040056e <+19>:	b8 00 00 00 00	mov    $0x0,%eax

28	}
   0x0000000000400573 <+24>:	5d	pop    %rbp
   0x0000000000400574 <+25>:	c3	retq   

End of assembler dump.
```
对于上面的输出，介绍一下`0x000000000040055b <+0>:	55	push   %rbp`各个字段的含义。
```
- 0x000000000040055b: 该指令对应的虚拟内存地址
<+0>: 该指令的虚拟内存地址偏移量
55: 该指令对应的计算机指令
push %rbp: 汇编指令
```
其实 main 函数并不是程序并不是程序拉起后的第一个执行的函数，main 函数也是一个被调函数。它被 _start 函数调用，这里不深究。只需要知道 main 函数是也被一个叫 _start 的函数调用的即可。这里也先不分析下面两行。
```
push   %rbp
mov    %rsp,%rbp
```
_start 函数执行时，栈上情况大致如下图所示，我们用绿色表示正在执行的函数的栈帧：
![](http://upload-images.jianshu.io/upload_images/9966001-7f5e71e8a110f05a.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
当 _start 函数调用了这里的 main 函数后，根据上面给出的输出结果中`=>`的位置来看，这时的 main 函数刚刚开始执行，栈上的情况大致如下图所示：
![](http://upload-images.jianshu.io/upload_images/9966001-f57a1535f2b27a04.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

执行以下命令，执行 3 行汇编代码。
``` shell
si 3
```
由于执行完 `start` 命令后，程序停在 `0x000000000040055c` 位置，所以这里执行以下三行代码
```
mov    $0x4,%esi
mov    $0x2,%edi
callq  0x400520 <compute>
```
一个函数调用另一个函数，需先将参数准备好。main 函数调用 compute 函数，所以前两行代码就是将两个参数传入通用寄存器中，对于参数传递的方式，x86和x86_64定义了不同的[函数调用规约(calling convention)](http://www.unixwiz.net/techtips/win32-callconv-asm.html)。相比x86_64将参数传入通用寄存器的方式，x86则是将参数压入调用栈中。这又是另一个专题了，不做讨论，接下来就要执行 call 指令了。
```
callq  0x400520 <compute>
```
这是一条 call 指令，call 指令要完成两个任务。
1. 将主调函数 main 中的下一条指令（callq 的下一条指令）所在的虚拟内存地址压入栈中（这里为 0x000000000040056e ）压入栈中，被调函数（compute）返回后将取这个地址的指令继续执行。时时刻刻要注意，每次入栈操作，rsp 寄存器的值都是会更新的。
2. call 指令会更新 rip 寄存器的值，使其值为被调函数（compute）所在的起始地址，这里为 0x400520。

当 call 指令执行完成后，这个时候，程序就执行到 compute 函数里了。我们仍旧使用以下命令查看当前函数的汇编信息。
``` shell
disassemble /rm
```
显示的结果如下：
```
Dump of assembler code for function compute:
18	{
=> 0x0000000000400520 <+0>:	55	push   %rbp
   0x0000000000400521 <+1>:	48 89 e5	mov    %rsp,%rbp
   0x0000000000400524 <+4>:	48 83 ec 18	sub    $0x18,%rsp
   0x0000000000400528 <+8>:	89 7d ec	mov    %edi,-0x14(%rbp)
   0x000000000040052b <+11>:	89 75 e8	mov    %esi,-0x18(%rbp)

19	    int e = add(a, b);
   0x000000000040052e <+14>:	8b 55 e8	mov    -0x18(%rbp),%edx
   0x0000000000400531 <+17>:	8b 45 ec	mov    -0x14(%rbp),%eax
   0x0000000000400534 <+20>:	89 d6	mov    %edx,%esi
   0x0000000000400536 <+22>:	89 c7	mov    %eax,%edi
   0x0000000000400538 <+24>:	e8 b0 ff ff ff	callq  0x4004ed <add>
   0x000000000040053d <+29>:	89 45 f8	mov    %eax,-0x8(%rbp)

20	    int f = mul(a, b);
   0x0000000000400540 <+32>:	8b 55 e8	mov    -0x18(%rbp),%edx
   0x0000000000400543 <+35>:	8b 45 ec	mov    -0x14(%rbp),%eax
   0x0000000000400546 <+38>:	89 d6	mov    %edx,%esi
   0x0000000000400548 <+40>:	89 c7	mov    %eax,%edi
   0x000000000040054a <+42>:	e8 b8 ff ff ff	callq  0x400507 <mul>
   0x000000000040054f <+47>:	89 45 fc	mov    %eax,-0x4(%rbp)

21	    return e * f;
   0x0000000000400552 <+50>:	8b 45 f8	mov    -0x8(%rbp),%eax
   0x0000000000400555 <+53>:	0f af 45 fc	imul   -0x4(%rbp),%eax

22	}
   0x0000000000400559 <+57>:	c9	leaveq 
   0x000000000040055a <+58>:	c3	retq   

End of assembler dump.
```
执行以下命令，将 rbp 寄存器中的地址入栈，然后将 rsp 中的地址 赋值给 rbp 寄存器（也就是让 rbp 指向当前 rsp）。
``` shell
si 2
```
此时栈上的情况如下图所示：

![](http://upload-images.jianshu.io/upload_images/9966001-9300ee815e44e800.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

接下来要执行的语句就是下面这条语句。这条语句的含义是**栈帧扩展**。我们上文提到过，栈从高地址向第地址生长，所以减操作是栈的扩展操作。这里就是为被调用的函数的栈帧预先开辟空间，空间大小为24个字节。到这里就分析完成了，接下来就是 compute() 函数的执行过程了。个人能力有限，有些地方展开说不清楚，大家可以自行更深入的研究一下。
```
sub    $0x18,%rsp
```
这里补充一个概念，计算机是按字节编址，按字节编址的含义就是说每个地址对应一个字节。64 位操作系统，每个地址由 8 个字节表示。
