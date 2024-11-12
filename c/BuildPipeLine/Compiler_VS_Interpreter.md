# Compiler ( 编译器 )
编译器是把程序的**==每一行代码==** 都 翻译成机器语言，无论 其中哪一行，**==在运行时是否==**会出错与否，**==都==**把它 翻译成 对应的机器语言

e.g.  // 所有行都会被 进行语法检测，并翻译成 机器语言

```C

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	int a = atoi( argv[1] );
	int b = atoi( argv[2] );

	// this line maybe occur  run-time error , if the value of b is zero
	double d_Result = (double)( a / b );
	printf("a / b = %d / %d = %f\n", a,b, d_Result);

	double double_Result = 2 * d_Result;
	printf("doube Result = %f\n",  double_Result );
	return 0;
}




```



# Interpreter ( 解释器 )
编译器是把程序的**==逐行==** **==(从文件的第1行开始,到最后一行)==** 都 1行接1行的翻译 成机器语言， 然后令CPU 执行这段机器代码，如果这段机器代码 正常执行成功了，那么，再 重复这一过程，翻译下一行
也就是说，即使 第2行，有语法错误，第1行也会被翻译并执行 , 但如果第1行，在执行过程中，出错了，第2行连翻译成机器语言的机会，都没有 


e.g.

```C

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
	int a = atoi( argv[1] );  // 第1行: 被翻译成机器语言，并由CPU执行机器代码， 一旦 命令行参数 少于2个时，此行会出现 Run-Time-Error , 第2行，连 翻译的机会都没有 
	int b = atoi( argv[2] )   // 第2行: the statement has a grammar error, It missed an semicolon at the end of the sentence 

	// this line maybe occur  run-time error , if the value of b is zero
	double d_Result = (double)( a / b );
	printf("a / b = %d / %d = %f\n", a,b, d_Result);

	double double_Result = 2 * d_Result;
	printf("doube Result = %f\n",  double_Result );

	int* pointer_to_int = (int*)malloc( sizeof(int) ); *pointer_to_int = 321;
	return pointer_to_int; // line 12  : 这1行 , 不会被 翻译器 提取出来，进行翻译，即使这是一行 不符合 int main(...) 语法约束的 syntax error statement , 它也没有机会 被翻译，因为第2行，已经有语法错误了，翻译失败，从来没有执行的机会， 第12行，就更加没有机会被 解释器 翻译了 
}


```


