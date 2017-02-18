#include <stdio.h>
#include <pthread.h>
void* myThread1(void)
{
        int i;
        for(i=0; i<10; i++)
        {
                printf("This is the 1st pthread \n");
                sleep(1);
        }
}

void* myThread2(void)
{
        int i;
        for(i=0; i<10; i++)
        {
                printf("This is the 2st pthread \n");
                sleep(1);
        }
}

int main()
{
        int i =0;
        int ret = 0;
        pthread_t id1,id2;
        
        //创建线程函数
        ret = pthread_create(&id1, NULL, (void*)myThread1,NULL);
		printf("%d\n",ret);
        if(ret)
        {
                printf("Create pthread1 error \n");
                return 1;
        }

        ret = pthread_create(&id2, NULL, (void*)myThread2,NULL);
		printf("%d\n",ret);
        if(ret)
        {
                printf("Create pthread2 error \n");
                return 1;
        }
        //以阻塞的方式等待id1指定的线程结束。
		printf("wating for thread1\n");
        pthread_join(id1, NULL);
		printf("wating for thread2\n");
        pthread_join(id2, NULL);
        return 0;
}
