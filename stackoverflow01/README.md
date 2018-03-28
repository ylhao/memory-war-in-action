#### 确定 bingo 函数返回地址

###### 读 elf 文件
```
readelf -a ./a.out | grep bingo
```

###### gdb 调试
```
gdb ./a.out
p bingo
quit
```

#### 偏移计算
计算 main 函数返回地址和 buffer 首地址在栈上的偏移，偏移 = main 函数返回地址在栈上的位置 - buffer 首地址
```
gdb ./a.out
start
p buffer  # 打印 buffer 内容
p &buffer # 打印 buffer 地址
info f  # 查看 rip
quit
```

#### 攻击脚本
```
// 安装 zio 库
sudo pip install zio
```

#### 运行样例
```
python exploit.py
```
