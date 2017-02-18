#include <pthread.h>
#include <stdio.h>
pthread_mutex_t count_mutex;
long long count;

void
increment_count()
{
	    pthread_mutex_lock(&count_mutex);
	    count = count + 1;
	    pthread_mutex_unlock(&count_mutex);
}

long long
get_count()
{
    long long c;
    
    pthread_mutex_lock(&count_mutex);
	    c = count;
    pthread_mutex_unlock(&count_mutex);
	    return (c);
}
void *printHello(void *args){
	int tid;
	tid = *(int *)args;
	printf("Hello this is thread %d\n",tid);
	pthread_exit(NULL);
	
}
int main(){
	pthread_t threads[5];
	int rc,t;
	for(t=0;t<5;t++){
		printf("in main thread create thread %d\n",t);
		rc = pthread_create(&threads[t],NULL,printHello,&t);
	}
	return 0;
}
