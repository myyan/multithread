#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>

//这里我模拟的是 每个线程的互斥锁
//如果某个线程 可以被抓取，那个student线程 会拿到抓取那个老师线程的 互斥锁 然后修改其数据 
pthread_mutex_t mutex[10];

//这里模拟的是 student number 和 marker number
struct demo_parameters {
	int S;
	int M;
};

//这里预留字段
struct demo_srelation{
	int sid;
	int mid;
};

//这里是 marker 和 student 的关联关系，主要是用来抓取marker线程用，和在marker线程里面打印 student线程id 
//一下字段都是在student线程里  遍历的时候去modify的  因为这些字段在多线程共享  所以 如果能够被住区 student 要加锁然后 去修改
//其中finish 字段是在 marker线程里修改的 
struct demo_mrelation{
	//该marker是否结束
	bool finish;
	//如果没有结束 那么查看该字段是否是空闲的
	bool free;
	//该marker的线程id
	int mid;
	//抓取该marker的student 线程id 
	int sid;
};
//预留字段
struct demo_srelation srelation[10];

//这就是 student 线程去遍历的  共享变量数组 查看哪些是空闲的且没结束的  如果是的话，修改共享变量 直接抓取
struct demo_mrelation mrelation[10];

struct demo_parameters parameters;

bool grabMarker(int sid){
	int i;
	int count = 0;
	for(i=0;i<10;i++){
		bool finish = mrelation[i].finish;
		bool free = mrelation[i].free;
		if(!finish&&free){
			mrelation[i].sid = sid;
			count++;
			if(count==3){
				break;
			}
		}
	}
	return true;
}

bool haveTime(){
	return true;
}
	/*
	这里的共享变量是每一个老师
	这里 student 在获取共享变量的时候 有可能被阻塞
	阻塞的时候 这个线程什么都不做 等待被唤醒  
	那么这里面唤醒是怎么实现呢？
	就是用老师线程完成后会进行通知
	
	如果学生获取了需要的所有老师 那么接下来的问题就是
	do something  占用这些老师 固定的时间
	然后结束前 不要急着释放锁  应该把占用的共享变量的信息 初始化
	然后再释放所 

	其实开始获取共享变量的时候也是这样的
	获取了共享变量的锁  然后再初始化 资源  

	然后 做完后 先初始化 然后再释放锁
	这样保证其他线程看到的都是正确的数据 其他线程 也就是 其他学生线程  
*/
void *studentT(void *argv){
	int studentId = *(int *)argv;
	printf("current student :%d\n",studentId);
	//学生找到 一定数量的老师才能开始汇报 老师是共享资源 需要同步
	bool ok = grabMarker(studentId);
	bool havetime = haveTime();
	if(ok&&havetime){
		printf("ok\n");
	}
}

/*
	老师线程 其实任务也不多
	但是还是有难点的，难点主要在于：
	1，如果student线程结束了 那么怎么通知marker线程？
	2，因为marker线程需要处理 特定数量的student线程  然后才推出 那么 应该是用while 来实现
	当听了特定数量学生的汇报后 就可以离开 这里 就可以  break 
	但是
	
	因为在 marker 线程里面需要打印 student 线程的id  
	那么 如何获取 student 线程的id呢？ 这里面就应该使用 全局变量 来实现，全局变量保存
	我准备用结构体数组来实现
	为每个marker线程来创建一个全局变量
	保存一下 几个字段
	1.studentId  保存的是 grab marker 线程的 student线程id
	2.markerId 保存的是 当前marker线程的id  其实 这个字段不用也行，因为我们用结构体数组的下标就表明了marker线程 id
	因为每个线程都有一个 全局变量
	3.isFinished 是否结束 : 结束的意思是 该老师已经听完了特定数量的报告
	如果结束了 student去grab线程的时候 发现这个字段是true 就直接continue 
	4.isFree 如果isFinished 是 false  那么 就 判断 isFree  如果是自由的(也就是说 没有被其他线程占用) 那么该marker线程就会被grab
	此时 student 抓取线程的数量会加1 如果达到特定的数量 ok  就 去运行 运行前要去修改共享的变量 
	是这样的  student 去grab的时候 通过共享变量 来遍历，遍历如果 能被抓取
	那就 加上锁 因为有个 加锁的数组  每个下标代表每个线程的 锁
	
*/
void *markerT(void *argv){
	int markerId = *(int *)argv;
	int count =0;
	//use count to simulation  but the main problem is how to be waked up by the student thread
	//use some mechanism to deal with
	int i;
	while(1){
		//这里应该被锁住  只有 指定的student 唤醒才行 因为需要等到 student grab完所需要的老师(指定数量)才行,而且即使student grab所有需要的
		//老师后 还是需要 判断能不能运行 所以  应该等待student 准备完毕后才行
		/*
			因为student线程 在抓取老师的时候 如果抓取成功会锁定这个marker线程  
			这里面 这里也用这个锁 然后 用condition 来唤醒
		*/
		printf("current marker:%d is running\n",markerId);
		printf("was grabbed by the student :%d\n",mrelation[markerId].sid);
		count++;
		if(count>=10){
			mrelation[markerId].finish = true;
			break;
		}
	}
}
int main(int argc,char *argv[]){
	
	if(argc<2){
		printf("parameters error\n");
	}
	parameters.S = atoi(argv[1]);
	parameters.M = atoi(argv[2]);
	int S = parameters.S;
	int M = parameters.M;
	int i;
	if(S>100 || M>100){
		puts("Maximum 100 markers and 100 students allowd\n");
		exit(1);
	}
	printf("SN : %d ,MN: %d \n",S,M);

	for(i=0;i<10;i++){
		mutex[i] = PTHREAD_MUTEX_INITIALIZER;
	}

	pthread_t student[S];

	pthread_t marker[M];
	int studentId[S];
	int markerId[M];
	//SN : student number  MN  : marker number
	for(i=0;i<S;i++){
		studentId[i] = i;
		pthread_create(&student[i],NULL,studentT,&studentId[i]);
	}

	for(i=0;i<M;i++){
		markerId[i] = i;
		pthread_create(&marker[i],NULL,markerT,&markerId[i]);
	}

	  /* Wait for student threads to finish */
    for (i = 0; i<S; i++) {
        pthread_join(student[i], NULL);
    }

    /* Wait for marker threads to finish */
    for (i = 0; i<M; i++) {
        pthread_join(marker[i], NULL);
    }
	return 0;

}
