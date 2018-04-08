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
