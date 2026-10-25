> 搬运来源：CSDN · 小木子先生（HELLO_LW）
> 原文地址：https://blog.csdn.net/HELLO_LW/article/details/130761423

# memcpy与memmove

## memcpy与memmove

笔者在看项目源码的时候发现了STL底层大量的使用了memmove函数，由于笔者能力有限，之前只使用过memcpy，所以相对这两个函数做一个对比进行一下记录。

```cpp
void* myMemcpy(void* des, const void* src, int count);
void* myMemmove(void* des, const void* src, int count);
```

这是我手动实现的两个函数原型，两者的功能都是一样的，将src中的count个内存拷贝到des中去，非常的使用，下面说一下区别。

memmove会考虑到内存重叠的问题

举个例子

```cpp
// 此代码为手写，并没有上编译器运行
char a[] = "12345678910";
char* b = a + 2;//指向3
memmove(b, a,  5);

char aa[] = "12345678910";
char* bb = aa + 2;//指向3
memcpy(bb, aa,  5);
```

这个例子中有3个字节重叠了，你可以思考一下这里的结果，肯定是不一样的，留下悬念，这里不给答案。

讲完了区别来看一下实现：

### memcpy实现

```cpp
void* myMemcpy(void* des, const void* src, int count)
{
	assert(des != nullptr && src != nullptr);
	if (des == src)
		return des;

	char* p = (char*)des;
	char* q = (char*)src;
	while (count--)
	{
		*p++ = *q++;
	}

	return des;
}
```

### memmove实现

```cpp
void* myMemmove(void* des, const void* src, int count)
{
	assert(des != nullptr && src != nullptr);
	if (des == src)
		return des;
	
	char* p = (char*)des;
	char* q = (char*)src;
	if (p < q || q + count <= p)
	{
		while (count--)
		{
			*p++ = *q++;
		}
	}
	else
	{
		p += count;
		q += count;
		while (count--)
		{
			*--p = *--q;
		}
	}
	return des;
}
```

可以清晰的看见在memmove中多了一个if，是用来判断是否存在内存叠加的；

### 代码测试

输入一些测试代码进行测试：

```cpp
int main()
{
	char a[8] = "abcdefg";

	memcpy(&a[1], &a[0], 4);
	std::cout << "memcpy a = " << a << std::endl;

	strcpy_s(a, sizeof("abcdefg"), "abcdefg");
	myMemcpy(&a[1], &a[0], 4);
	std::cout << "mymemcpy a = " << a << std::endl;

	strcpy_s(a, sizeof("abcdefg"), "abcdefg");
	memmove(&a[1], &a[0], 4);
	std::cout << "memmove a = " << a << std::endl;

	strcpy_s(a, sizeof("abcdefg"), "abcdefg");
	myMemmove(&a[1], &a[0], 4);
	std::cout << "mymemmove a = " << a << std::endl;
	return 0;
}

输出：
memcpy a = aabcdfg
mymemcpy a = aaaaafg
memmove a = aabcdfg
mymemmove a = aabcdfg
```

可以看到系统的memcpy跟字节写的mymemcpy是不一样的。所以我推断，编译器会自动对memcpy内存叠加问题进行优化，如果有叠加，就使用memmove去执行。
