#include <stdio.h>
#include <pthread.h>

int cond = 1;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thr_exit(){
	pthread_mutex_lock(&m);
	/* Cretical Region */
	cond = 0;
	pthread_cond_signal(&c);//signal the parent process
	pthread_mutex_unlock(&m);
}

void *child(void *arg){
	printf("child\n");
	thr_exit();
	return NULL;
}
void thr_join(){
	pthread_mutex_lock(&m);
	while(cond == 1)
		pthread_cond_wait(&c,&m);
	pthread_mutex_unlock(&m);
}
int main(){
	printf("parent:begin\n");
	pthread_t c;
	pthread_create(&c,NULL,child,NULL);
	thr_join();
	printf("parent:end\n");

	return 0;
}
