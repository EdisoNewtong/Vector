# 源代码出自 《Unix网络编程》( 卷2: 进程间通信 ) P130 ( git src hash-id : 8861e89 )

```Shell
$ ./producer_consumer.exe 1000000 5
Thread #1 : filled slot count[0] = 202370
Thread #2 : filled slot count[1] = 205286
Thread #3 : filled slot count[2] = 205412
Thread #4 : filled slot count[3] = 174361
Thread #5 : filled slot count[4] = 212571

    gCnt = 1000000, gCnt == nitems [OK]

$ ./producer_consumer.exe 1000000 10
Thread #1 : filled slot count[0] = 111018
Thread #2 : filled slot count[1] = 103485
Thread #3 : filled slot count[2] = 106308
Thread #4 : filled slot count[3] = 121457
Thread #5 : filled slot count[4] = 106186
Thread #6 : filled slot count[5] = 82450
Thread #7 : filled slot count[6] = 102498
Thread #8 : filled slot count[7] = 89120
Thread #9 : filled slot count[8] = 97523
Thread #10 : filled slot count[9] = 79955

    gCnt = 1000000, gCnt == nitems [OK]



$ ./producer_consumer.exe 100 5
Thread #1 : filled slot count[0] = 100
Thread #2 : filled slot count[1] = 0
Thread #3 : filled slot count[2] = 0
Thread #4 : filled slot count[3] = 0
Thread #5 : filled slot count[4] = 0

    gCnt = 100, gCnt == nitems [OK]

$ ./producer_consumer.exe 10000 5
Thread #1 : filled slot count[0] = 9221
Thread #2 : filled slot count[1] = 0
Thread #3 : filled slot count[2] = 779
Thread #4 : filled slot count[3] = 0
Thread #5 : filled slot count[4] = 0

    gCnt = 10000, gCnt == nitems [OK]
```

Q : 输出结果意味着什么 ?
可以看到，指定分配的线程数量之后( 都是分配5个线槽 )
打印出了各工作线程为 1000000 个数组元素 **==填充的实际槽位的个数==**
而这些线程填充的次数==**之和**==，就是==**总的数组的长度**==(槽位的总个数)

而当缩减少槽位总个数之后(100个) , #2~#5 ，就不一定有机会参与实际的填充数据的过程了，都被#1 Thread 给填充完了
与此同时，各个线程之间的切换和协同工作，也**==不一定是按照线程的创建顺序==**来依次执行的
从打印结果来看 #1 和 #3 执行了，而 #2 #4 #5 **==没有执行的机会==**
是**==真的没有机会执行==**吗？**==也不一定!!!==**
可能是由于以下的时间轴序列的运行流程

1. Thread #1 : 执行 84 ~ 97 行之间的 for 循环 (可能已经执行了 9100 次 )
1. Thread #1 : CPU 时间片到期后，(停留在 85 ~ 93之间的某一行 ) , CPU切换时间片，交给 Thread #2 执行
1. Thread #2 : 从 void* produce(void* arg) 开始执行，但阻塞在 pthread_mutex_lock(...) 处，因为 Thread #1 已经锁定( 拥有此 互斥锁 的所有权 ) 了 mutex
1. Thread #1 : 由于 Thread #2**==已经被阻塞==**，所以 CPU又将时间片给 Thread #1继续执行， 又执行了121次，共 9100+121=9221次后，在 95行处( pthread_mutex_unlock(...) 已经执行完毕了 )，时间片到期了，于是将执行权 交给 Thread #3来执行
1. Thread #3 : 由于互斥量已经解锁，所以 Thread #3 进入 for循环，并获取了 mutex的所有权， 然后执行了 779次 填充行为后， 到达了数组的上限，解锁 mutex并退出了 for循环， Thread #2的线程函数 **==执行终止==**
1. Thread #? : 此时无论 CPU将执行时间片分配给 Thread #1 or #2 or #4 or #5 都会进入 for 循环后，执行 86 ~ 88行的逻辑，到达数组下标的上限后，解锁 mutex，并且到达 ==**线程执行的终点**== ， 第88行

至此，所有 #1 ~ #5 线程执行都已经终止
输出各个线程，操控数组 buff 的填充次数 

Q : **==以下代码的  彻底删除 lock / unlock , 会对运行结果有何影响==**
一定会导致 多个共享相同代码片段的同一个线程函数 , 可能会导致

 shared.buff[1] = 0;
 shared.buff[2] = 0;
 shared.buff[3] = 0;
 // 总之 shared.buff[i] != i 的情况会时有发生

or 
 shared.buff[2] = 3;
 shared.buff[2] = 4;
 shared.buff[2] = 5;



Q : **==以下代码的  lock / unlock==**

pthread_mutex_lock(...)
	...
    ...
	...
pthread_mutex_unlock( ... )

==**当 lock 与 unlock 中间夹着不同的代码块时，会对 运行结果 有何种深远的影响？**==
e.g.

```C

////////////////////////////////////////////////////////////////////////////////////////////////////
	pthread_mutex_lock(...)
	for ( ; ;  ) {
		   ....

        shared.nval++;

        *( (int*)arg ) += 1;
	}
    pthread_mutex_unlock( &shared.mutex );

////////////////////////////////////////////////////////////////////////////////////////////////////

	pthread_mutex_lock(...)
	for ( ; ;  ) {
		   ....

        shared.nval++;
        pthread_mutex_unlock( &shared.mutex );

        *( (int*)arg ) += 1;
	}


////////////////////////////////////////////////////////////////////////////////////////////////////

	for ( ; ;  ) {
		pthread_mutex_lock(...)
		   ....

        shared.nval++;

        *( (int*)arg ) += 1;
        pthread_mutex_unlock( &shared.mutex );
	}


```

  

```C
void* produce(void* arg)
{
    // <Infinite> for-loop style
    for( ; ; ) {
        pthread_mutex_lock(&shared.mutex);
        if ( shared.nput >= nitems ) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }

        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;
        pthread_mutex_unlock( &shared.mutex );

        *( (int*)arg ) += 1;
    }

}
```