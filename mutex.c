#include <stdio.h>
#include <pthread.h> 
#define N 10
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

void * myThread(void *arg){
	int id = *(int *)arg;	
	int i;
	printf("current thread is :%d\n",id);
	pthread_mutex_lock(&mutex);

	for(i=0;i<N;i++){
		printf("current thread :%d print\n",id);
		sleep(1);
	}
	pthread_mutex_unlock(&mutex);
}


int main(){

	pthread_t thread[N];
	int id[N],i;
	for(i=0;i<N;i++){
		id[i] = i;
		pthread_create(&thread[i], NULL, myThread, &id[i]);
	}

	for(i=0;i<N;i++){
		pthread_join(thread[i],NULL);
	}
	return 0;
	

}
