#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include<pthread.h>

pthread_mutex_t mutex[10];
struct demo_parameters {
	int S;
	int M;
};

struct demo_srelation{
	int sid;
	int mid;
};

struct demo_mrelation{
	bool finish;
	bool free;
	int mid;
	int sid;
};

struct demo_srelation srelation[10];

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
	2，marker线程 while的问题  

*/
void *markerT(void *argv){
	int markerId = *(int *)argv;
	int count =0;
	//use count to simulation  but the main problem is how to be waked up by the student thread
	//use some mechanism to deal with
	while(1){
		printf("current marker:%d is running\n",markerId);
		count++;
		if(count>=10){
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
