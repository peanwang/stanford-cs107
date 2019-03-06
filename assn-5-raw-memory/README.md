
![1](https://img-blog.csdnimg.cn/20190226162422855.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MTI1NjQxMw==,size_16,color_FFFFFF,t_70)
# Abstract

本人正在学习[Stanford CS107](https://see.stanford.edu/Course/CS107)。这门课很不错，我简单把这门课的信息说一下。

|key|value
|--|--
|官网|https://see.stanford.edu/Course/CS107
|课程名| 编程范式
|所需基础| C,C++，数据结构与算法
|上课视频| Bilibili搜索 https://www.bilibili.com/video/av9789206 (可能会失效)
|我做的作业|https://github.com/peanwang/stanford-cs107

# Assignment Task

这次的作业是以回答问题为主，而不是编程，所有我把几个好的问题记录在此。

<br>

## Problem 1: Binary numbers and bit operations
①  使用bit operations(与或非，异或)计算一个数除以4的余数？
把该数用二进制表示，会发现高位都是4的倍数，只有第一位和第二位不是，(1,2,4,8,16)
所以答案是
```cpp
Num & 3
```
同理：计算一个数除以2的余数，
```cpp
Num & 1
```
在LeetCode的数学分类里：有很多这样的bit题。

② 使用bit operation，去数的相反数
相反数的操作是 取反+1，即
```cpp
(num^-)+1
//或者
~num+1
```
③设a，b两数
有 (a ^b) ^b = a
```cpp
#include <stdio.h> 
#define KEY ‘Z’ 
int main(void)  
{ 
	int ch; 
	while ((ch = getc(stdin)) != EOF) 
		putc(ch ^ KEY, stdout); 
	return 0; 
}
```

<br>

## Problem 3: It's just bits and bytes 
这个问题是使用gdb查看内存。查看内存的命令是
```html
x/FMT ADDRESS
FMT ：  n 打印几个单位
format     o(octal). x(hex) d(decimal) u(unsigned) t(binary) f(float)  c(char) 
b(bytes)  h(halfword) w(word)  g(double word)

```

<br>

## Problem 4: Identical Outputs 
```assembly
R2 = Mem[SP] 
R3 = R2 + 4 
R4 = Mem[SP + 4] 
Mem[R4] = R3 
```
问题a:
```c
	char **a;                        // Mem[SP+4]
	char *b = strdup("abcdef");		// Mem[SP]
	*a = b+4;						// R2 = Mem[SP]
									// R3 = R2+4
									// R4 = Mem[SP+4]
									// Mem[R4] = R3 	
```
问题b
```c
int array[2];
*(int*)a[1] = a[0]+4;
```

<br>

## Problem 5: Find the linked list 
```cpp
const char* end = (char*)kHeapStart + kHeapSize - sizeof(list);
 
bool isListOnHeap(){
	const char* walk = (char*)kHeapStart;
	while(walk<=end){
		const list *current = (list*)walk;
		if(current->data == 1 && isInHeap(current->next)){
			current = current->next;
			if(current->data == 2 && isInHeap(current->next)){
				current = current->next;
				if(current->data == 3)
					return true;
			}
		}
		walk++;
	}
	return false;
}
static bool isInHeap(const char* Ptr){
	return Ptr>=kHeapStart && Ptr<=end;
}
```

<br>

## Problem 6: Homestar Runner: The System Is Down 
a)
```assembly
SP = SP-4          //char bubs[4]

R1 = Mem[SP+8]     //strongmad.coachz
R1 = R1*4               //strongmad.coachez * sizeof(short*) 
R2 = Mem[SP+24]    // marzipan
R2 = R2 + 4            //  marzipan->thecheat
R2 = R2 + R1      //marzipan->thecheat[strongmad.coachz]
R3 =  .2 Mem[R2]      
R4  = .1 Mem[SP]    //*bubs
R5 = R4 + SP   
Mem[R5]  = .1 R3

R1 = Mem[SP]           //*(int*) bubs
R1 = R1*4                  //   加上int指针
R2 = SP+24 
R3 = Mem[R2] 
R3 = R3+R1
Mem[R2+R2] = R3

SP = SP+4
RET
```
b)
```assembly
R1 = Mem[SP+4]        //marshie
R2 = Mem[SP+8]       //mrshmallow
SP = SP-8 
Mem[SP+4] =  R1
Mem[SP] =  R2
CALL <puppetthing>

SP = SP+8
R1 = RV
R1 = Mem[R1]  
R1 = Mem[R1 + 12]
RV = R1 

RET
```

<br>

## Problem 7: The Hitchhiker’s Guide To The Galaxy 
```assemply
R1 = .1 Mem[SP+8]         //*marvin.ford
R1 = R1 * 4                     // (*marvin.ford)*sizeof(int)
R2 = Mem[SP+36]           // LOAD deepthought
R3 = R2 + R1 
R2 = Mem[R3]          //  deepthought[*marvin.ford]
R3 = SP + 4 + 4 + 12 + 4 +100*2          //(SP+224) 
Mem[R3] = .2 R2

R1 = Mem[SP+4]
R1 = Mem[R1]
R1 = .2 Mem[R1]              //**marvin.arthur  
R2 = SP+24                 // marvin.zaphod
R2 = R2 + 4               	 // ((galaxy *)(marvin.zaphod))->ford    
R2 = R2 + 20        		// ((galaxy *)((galaxy *)(marvin.zaphod)->ford) ->trillian
Mem[R2] = R1

R1 = Mem[SP+4]             //marvin.arthur
R2 = SP+4						//    &marvin
R2 = R2 + sizeof(galaxy)    //  &marvin+1
SP = SP-8
Mem[SP] =  R2
Mem[SP+4] = R1
CALL <hithhikersguide>

SP = SP+8
R1 = Mem[RV]
R2 = 40
R1 = Mem[R1+R2]
RV = R1
RET
```


## Problem 8: C++’s Dark Side
别忘记了成员函数有一个隐藏参数this

<br>

```assembly
R1 = Mem[SP+8]           // macewindu -> council
R2 = 40*2                      // short[40]
R3 = .2 Mem[R1+R2]       // macewindu->council[40]
R3 = R3*4                   //short*  
R4 = Mem[SP+20]      // obiwan.yoda
R4 = R4 + R3
Mem[SP+20] = R4

R3 = SP+12                   // this
R1 = Mem[SP+4]        // this
R2 = SP+12                 // & ojiwan
SP = SP-12
Mem[SP] = R3
Mem[SP+4] = R1 
Mem[SP] =  R2
CALL <jedimaster::anakin>

SP = SP+12
RV = Mem[RV]
RET
```

<br>

# Assignment think
经过C转汇编的折磨后，我对内存和指针有了更深的认识。(⊙ˍ⊙)

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190227212135883.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MTI1NjQxMw==,size_16,color_FFFFFF,t_70)
①：为什么只能取变量一次地址，而不能无限。
用C语言来讲：&num是一个表达式，不能取地址
用汇编来讲：因为每个变量都会有一个地址，而该地址只是一个值(表达式)，而不是变量(变量有地址，值(表达式)没有)。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190227212356954.png)
```c
int a = 10;
printf("%x",&(char*)a);
```
同理，上面的代码也会出错，因为(char*)a是一个表达式，得出来的结果是个值，而不是变量。有了上面的基础。就可以看看Problem 7中的一个typecast了。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190228110050802.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MTI1NjQxMw==,size_16,color_FFFFFF,t_70)

上面是Problem 7里一段代码：其中line 2
```cpp
marvin.zaphod           // 类型short*,值为  SP+24
(galaxy*)(marvin.zaphod)//  类型galaxy*,值为 SP+24
(galaxy*)(marvin.zaphod)->ford//  类型char*,值为 SP+32
((galaxy*)((galaxy*)(marvin.zaphod)->ford)->trillian//  类型int,值为 Mem[SP+48]
```
请看下面的例子：
```cpp
typedef struct {
	int a;
	int b[2];
}test;
int main()
{
	test t;
	t.a = 1;
	t.b[0] = 0;
	t.b[1] = 1;
	printf("%d",((test*)(t.a))->a);     //wrong    int->test*
	printf("%d",((test*)(t.b))->a);	    // yes     int*->test*
}
```
t.a     是int类型
t.b     是int*类型

<br>

②typecast：
无论怎么转型，数据还是一样的，只是编译器对它解释不同(导致生成不同汇编代码)。

**C语言为我们提供了int，double等基础数据类型，其实是对内存中的数据的一种抽象，所以无论怎么转型，一定要厘清转型后的数据类型是什么。**


