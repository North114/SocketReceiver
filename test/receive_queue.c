#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 256
#define DEBUG

struct msq_buf {
	long msq_type;
	unsigned char data[BUFFER_SIZE];
};

int main(){
	struct msq_buf mybuf;
	struct sembuf sem_buf = {0,0,SEM_UNDO};
	/*
	** Generate a Key
	*/
	key_t msq_key = ftok("../key",'q');
	key_t sem_key = ftok("../key",'s');
	
	int msq_id;
	int sem_id;
	
	int data_length;
	int msq_type;
	
	/*
	** Check Key Validity
	*/
	if(msq_key == -1 || sem_key == -1) {
		perror("Create Key Failed!");
		return 1;
	} else {
		#ifdef DEBUG
		printf("msq_key is : %x\n",msq_key);
		printf("sem_key is : %x\n",sem_key);
		#endif
	}

	/*
	** Get The Semaphore
	*/
	sem_id = semget(sem_key,1,0666 | IPC_CREAT);
	if(sem_id <= 0) {
		perror("Get Semaphore Id Failed!");
		return 2;
	} else {
		printf("sem_id is : %d\n",sem_id);
	}

	/*
	** Get The Message Queue
	*/
	msq_id = msgget(msq_key,0666 | IPC_CREAT);
	if(msq_id == -1) {
		perror("Get Message Queue Failed!");
		return 3;
	}

	/*
	** Acquire the Semaphore(sem_id,sembuffer,number of sembuffer)
	*/
	sem_buf.sem_op = -1;
	if(semop(sem_id,&sem_buf,1) == -1) {
		perror("Can't Acquire Semaphore");
		return 4;
	}
	/* Start of Restrict Zone */
	
	/*
	** Received Message from Message Queue
	*/
	printf("Please Enter Message Queue Type : ");
	scanf("%d",&msq_type);
	while((data_length = msgrcv(msq_id,&mybuf,BUFFER_SIZE,msq_type,IPC_NOWAIT)) > 0){
		mybuf.data[data_length] = '\0';
		printf("Received Message : %s\n",mybuf.data);
	}
	
	/* End of Restrict Zone */
	/*
	** Release the Semaphore
	*/
	sem_buf.sem_op = 1;
	if(semop(sem_id,&sem_buf,1) == -1) {
		perror("Can't Acquire Semaphore");
		return 5;
	}

	return 0;
}
