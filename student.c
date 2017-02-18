#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

struct demo_parameters {
	int S;
	int M;
};

struct demo_relation{
	int S;:
	int M;
}

struct demo_parameters parameters;

void *studentT(void *argv){
	int studentId = *(int *)argv;
	printf("current student :%d\n",studentId);
}

void *markerT(void *argv){
	int markerId = *(int *)argv;
	printf("current marker :%d\n",markerId);
}
int main(int argc,char *argv[]){
	
	if(argc<2){
		printf("parameters error\n");
	}
	parameters.S = atoi(argv[1]);
	parameters.M = atoi(argv[2]);
	int S = parameters.S;
	int M = parameters.M;
	if(S>100 || M>100){
		puts("Maximum 100 markers and 100 students allowd\n");
		exit(1);
	}
	printf("SN : %d ,MN: %d \n",S,M);
	pthread_t student[S];
	pthread_t marker[M];
	int studentId[S];
	int markerId[M];
	//SN : student number  MN  : marker number
	int i;
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
