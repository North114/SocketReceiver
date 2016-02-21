#include <stdio.h>
#include <stdlib.h>    //strlen
#include <unistd.h>    //write
#include <string.h>    //strlen strcat

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/tcp.h> //for macro in setsockopt
#include <arpa/inet.h> //inet_addr
 
#include <pthread.h> //for threading , link with lpthread

#include <mysql/my_global.h>
#include <mysql/mysql.h>

#include <libgen.h>
#include "Parameters.h"

#define startByte 0xAA 
#define endByte 0x75
#define DEBUG_TIME

#define MSG_BUFFER_SIZE 256
#define SOCK_BUF_SIZE 256

void *connection_handler(void *);
void *sendDate_handler(void *);
void restore_data(unsigned char *,unsigned int);//??
unsigned char* stringProcess(unsigned char *,int*);
 
/* Key Define as Global Variable to be Accesse in function/thread */
key_t msq_key;
key_t sem_key;
key_t shm_key;

/* Message Queue Related Variable */
struct msq_buf {
	long msq_type;
	char data[MSG_BUFFER_SIZE];
};

int time_count = 0;

int main(int argc , char *argv[])
{   
    /* Socket Related Variable Declaretion */
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    //unsigned char *message;
    
	/* setsockopt related variable */
    int optval;

	/* shared memory key file */
	char key_dir[256];
	char *p_key = key_dir;
	p_key = dirname(dirname(dirname(getcwd(key_dir,sizeof(key_dir)))));
	/* Obtain a key for shared memory */
	strcat(key_dir,"/ipc/dan");
	#ifdef DEBUG_TIME
		printf("key directory is : %s\n",key_dir);
	#endif

	/*
	** Generate a Key
	*/
	msq_key = ftok(key_dir,'q');
	sem_key = ftok(key_dir,'s');
	shm_key = ftok(key_dir,'z');//generate a key
	
	#ifdef DEBUG_TIME
		printf("shared memory key is : %x\n",shm_key);
	#endif
	
	/*
	** Check Key Validity
	*/
	if(msq_key == -1 || sem_key == -1 || shm_key == -1) {
		perror("Create Key Failed!");
		return 1;
	} else {
		#ifdef DEBUG_TIME
		printf("msq_key is : %x\n",msq_key);
		printf("sem_key is : %x\n",sem_key);
		printf("shm_key is : %x\n",shm_key);
		#endif
	}

    //Create socket 
	//socket_desc refers to "socket descriptor"
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);//parameter:ipv4 protocol,full-duplex stream socket,TCP protocol
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;//ipv4 protocol
    server.sin_addr.s_addr = inet_addr(SERVER_IP);//ip address(now this address is the address of my server )
    server.sin_port = htons(PORT_NUMBER);//port number
     
    /* set keepalive option */
    optval = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));//open keep-alive
    optval = 5;
    setsockopt(socket_desc, SOL_TCP,TCP_KEEPCNT, &optval, sizeof(int));//number of probe
    optval = 300;	//5 minutes larger than current heart beat packet interval - 30 seconds
    setsockopt(socket_desc, SOL_TCP, TCP_KEEPIDLE, &optval, sizeof(int));//keep-alive time(second)
    optval = 5;
    setsockopt(socket_desc, SOL_TCP, TCP_KEEPINTVL, &optval, sizeof(int));//probe interval

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);//parameter:socket descriptor,maximum link number
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    //once accept a socket connection,system will reallocate a socket descriptor -- new_socket
    //the following statement using "=" but not "==",so this is a infinity loop
    //deadloop
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        puts("Connection accepted");
         
        //Reply to the client
        //message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
        //write(new_socket , message , strlen( message ));
         
        pthread_t sniffer_thread;//a unsigned int number,which used as thread identifier
		pthread_t sendding_thread;//thread for sending data to client

        new_sock = malloc(1);
        *new_sock = new_socket;
        
		/* Create a Thread to Receive Data from Client */
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0) {
            perror("could not create thread - connection_handler");
            return 5;
        }
	
		/* Create a Thread to Sending Data to Client */
        if( pthread_create( &sendding_thread , NULL , sendDate_handler , (void*) new_sock) < 0) {
            perror("could not create thread - sendDate_handler");
            return 6;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (new_socket < 0) {
        perror("accept failed");
        return 7;
    }
     
    return 0;
}
 
/*
** prototype function to be executed by  the thread,
** This will handle connection for each client.
*/
void *connection_handler(void *socket_desc) {
    //Get the socket descriptor
    int i;
    int sock = *(int*)socket_desc;
    int read_size;
    unsigned char client_message[SOCK_BUF_SIZE];
    unsigned char *temp;
	
    while( (read_size = recv(sock , client_message , SOCK_BUF_SIZE , 0)) > 0 ) {
        client_message[read_size] = '\0';
		
		temp = stringProcess(client_message,&read_size);
		if(read_size >= 8 && temp != NULL){
			restore_data(temp,read_size);
            #ifdef DEBUG_TIME
				printf("Received Valid String:%s \n",client_message);
            #endif
			//*(client_message) = '\0';
		}
		else {
            #ifdef DEBUG_TIME
    			printf("Invalid string:");
    			for(i = 0;*(client_message + i) != '\0';++i) {
    				printf("%x ",*(client_message + i));
    			}
    			printf("\n");
            #endif
			//*(client_message) = '\0';
		}
        read_size = 0;//clear read size
    }
     
    if(read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if(read_size == -1) {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
	return 0;
}
/*
** Thread that Sending Data to Client 
**
*/
void *sendDate_handler(void *socket_desc){
    int sock = *(int*)socket_desc;
    int write_status = 0;
	int i = 0;

	/* Message Queue and Semaphore Related Variable Declaration */
	struct msq_buf mybuf;
	int msq_id;
	int data_length;
	int msq_type = 1;

	struct sembuf sem_buf = {0,0,SEM_UNDO};
	int sem_id;
	
	/* Shared Memory Related Variable Declaration */
	int ShmID;
	char *ShmPTR;
	
	/*
	** Get The Semaphore
	*/
	sem_id = semget(sem_key,1,0666 | IPC_CREAT);//if semaphore does't exist , we will create it 
    //but semaphore size is 1 not 3 , so php script can't access it , so we need to run php
    //script first
	if(sem_id <= 0) {
		perror("Get Semaphore Id Failed!");
		return 0;
	} else {
		printf("sem_id is : %d\n",sem_id);
	}
	
	/*
	** Get The Message Queue
	*/
	msq_id = msgget(msq_key,0666 | IPC_CREAT);
	if(msq_id == -1) {
		perror("Get Message Queue Failed!");
		return 0;
	}

	/* 
	** Get The Shared Memory
	*/
    ShmID = shmget(shm_key,100,0666 | IPC_CREAT);
	if(ShmID <= 0){
		perror("Get Shared Memory Id Failed");
		return 0;
	} else {
		printf("ShmID is : %x\n",ShmID);
	}
    ShmPTR = (char *)shmat(ShmID,NULL,0);//attach a pointer to point at shared memory

	while(1) {
        /*
        ** Acquire the Semaphore(sem_id,sembuffer,number of sembuffer)
        */
        sem_buf.sem_op = -1;
        if(semop(sem_id,&sem_buf,1) == -1) {
            perror("Can't Acquire Semaphore");
            return 0;
        }
        /* Start of Restrict Zone */

        /*
        ** Received Message from Message Queue(msq type is 1)
        ** Here , We just fetch 1 message from message queue
        */
        
        data_length = msgrcv(msq_id,&mybuf,MSG_BUFFER_SIZE,msq_type,IPC_NOWAIT);
        mybuf.data[data_length] = '\0';
        
        if(data_length > 0) {
            printf("Received Data is : ");
            for(i = 0; i < data_length;++i){
                printf("%x ",mybuf.data[i]);
            }
            printf("\n");
            #ifdef DEBUG_TIME
            printf("Received Message : %s\n",mybuf.data);
            #endif
            /* index 21 indicate GPRS command sending status */
            if(*(ShmPTR + 21) != 's' && *(ShmPTR + 20) == 's') {
                /* Now,we start query for Data */
                *(ShmPTR + 21) = 's';
                
                write_status = write(sock , mybuf.data, strlen(mybuf.data));
        
            } else {
                #ifdef DEBUG_TIME
                printf("Already Sent Query Command");
                #endif
            }
        }
        /* End of Restrict Zone */
        /*
        ** Release the Semaphore
        */
        sem_buf.sem_op = 1;
        if(semop(sem_id,&sem_buf,1) == -1) {
            perror("Can't Acquire Semaphore");
            return 0;
        }
        
        if(write_status == -1){
            perror("Write Data to Client Failed!");
            return 0;
        }
        usleep(500000);//sleep for 0.5 second
	}

	return 0;
}

/* reurn the correct start pointer */
unsigned char* stringProcess(unsigned char* c,int* rec_len){
	/*	find the start point of a packet when packet is: 
	 *	1.inserted some byte
	 *	2.losted some byte
	 */
	int i;
	int len = *rec_len;

	if(c == NULL || rec_len == NULL || len == 0)return NULL;
	for(i = 0;i < len && c != '\0';++i,++c){
		if(*c == startByte)return c;
		--(*rec_len);//valid length
	}
	return NULL;
}

void restore_data(unsigned char *rec_data_package,unsigned int length){ 

	MYSQL *conn;//connect handle(struct st_mysql)
  	my_ulonglong affected_rows;//unsigned __int64
	unsigned char user_id[10] = "";	
	unsigned char current[10] = "";
	unsigned char voltage[10] = "";
    unsigned char zoneId[10] = "";
  	unsigned char query_statement_seg1[] = "INSERT INTO demo VALUES(";//change pointer on .data to array on heap
    unsigned char realtime_statement_seg1[] = "INSERT INTO RealTimeData VALUES(";
    unsigned char voltage_monitor_seg1[] = "INSERT INTO VoltageMonitor VALUES(";
	unsigned char query_statement[200] = "";
	unsigned int id,zoneId_value,current_value,voltage_value;

	/* Shared Memory Related Variable Declaration */
	int ShmID;
	char *ShmPTR;
    
    /* Semaphore Related Variable Declaretion */
	struct sembuf sem_buf = {0,0,SEM_UNDO};
	int sem_id;

	/* 
	** Get The Shared Memory
	*/
    ShmID = shmget(shm_key,100,0666 | IPC_CREAT);
	if(ShmID <= 0){
		perror("Get Shared Memory Id Failed");
		return;
	} else {
		printf("ShmID is : %x\n",ShmID);
	}
    ShmPTR = (char *)shmat(ShmID,NULL,0);//attach a pointer to point at shared memory
    
    /*
	** Get The Semaphore
	*/
	sem_id = semget(sem_key,1,0666 | IPC_CREAT);
	if(sem_id <= 0) {
		perror("Get Semaphore Id Failed!");
		return;
	} else {
		printf("sem_id is : %d\n",sem_id);
	}
	//validation
	if(*(rec_data_package) != startByte || *(rec_data_package + length - 1) != endByte){
        #ifdef DEBUG_TIME
            printf("invalid data:%x\n",rec_data_package[0]);
        #endif

        return;
    }
	//printf("valid data!!\n");
	//printf("%x,%x,%x,%x,%x,%x,%x\n",rec_data_package[0],rec_data_package[1],rec_data_package[2],rec_data_package[3],rec_data_package[4],rec_data_package[5],rec_data_package[6]);
	//reconstruct values
	id = rec_data_package[1];
	current_value = rec_data_package[2] * 256 + rec_data_package[3];
	//printf("current value:%d\n",current_value);
	voltage_value = rec_data_package[4] * 256  + rec_data_package[5];
    
    if(length == 9) zoneId_value = rec_data_package[7];
    else zoneId_value = 1;//default zoneId

	//printf("voltage value:%d\n",voltage_value);
	//*query_statement = "INSERT INTO GPRS_TEST (user_id,current_leak,voltage,rec_time) VALUES('0000','0.000','0.000','2000-01-01-00:00:00')";
	//construct query statement
	
	user_id[0] = 0x30 + id / 100;
	user_id[1] = 0x30 + (id % 100) / 10;
	user_id[2] = 0x30 + id % 10;
	user_id[3] = 0x2C;//comma
	user_id[4] = 0x00;//end of string
    
    zoneId[0] = 0x30 + zoneId_value / 100;
    zoneId[1] = 0x30 + (zoneId_value % 100) / 10;
    zoneId[2] = 0x30 + zoneId_value % 10;
    zoneId[3] = 0x2C;//comma
    zoneId[4] = 0x00;//end of string

	current[0] = 0x30 + (current_value / 10000);
	current[1] = 0x30 + ((current_value % 10000) / 1000);
	current[2] = 0x30 + ((current_value % 1000) / 100);
	//current[3] = 0x30 + (char)(current_value%10);
	current[3] = 0x2C;//comma
	current[4] = 0x00;//end of string

	voltage[0] = 0x30 + (voltage_value / 10000);
	voltage[1] = 0x30 + ((voltage_value % 10000) / 1000);
	voltage[2] = 0x30 + ((voltage_value % 1000) / 100);
	//voltage[4] = 0x30 + (char)(voltage_value%10);
	//voltage[3] = 0x2C;//comma
	voltage[3] = 0x00;//end of string

	//printf("%s\n",rec_data_package);
	//printf("%s\n",query_statement);


	/* DataBase link initialization */
	conn = mysql_init(NULL);
  	/* Connect DataBase */
  	mysql_real_connect(conn,DB_HOST, DB_USERNAME, DB_PASSWORD,DB_NAME, 0, NULL, 0);
	
    if(rec_data_package[6] == 0x01) {
		/* Store History Date */
        strcat(query_statement,query_statement_seg1);//(des,src)
        strcat(query_statement,user_id);
        strcat(query_statement,zoneId);
        strcat(query_statement,current);
        strcat(query_statement,voltage);
        strcat(query_statement,",");
        strcat(query_statement,"current_date,");
        strcat(query_statement,"current_time)");

        #ifdef DEBUG_TIME
            printf("%s\n", query_statement);
        #endif

        mysql_query(conn,query_statement);
    } else if(rec_data_package[6] == 0x50) {
		/* Store Realtime Data */
        strcat(query_statement,realtime_statement_seg1);//(des,src)
        strcat(query_statement,user_id);
        strcat(query_statement,zoneId);
        strcat(query_statement,current);
        strcat(query_statement,voltage);
        strcat(query_statement,")");

        #ifdef DEBUG_TIME
            printf("%s\n", query_statement);
        #endif

        mysql_query(conn,query_statement);
    } else if(rec_data_package[6] == 0x80) {
        /*
        ** Acquire the Semaphore(sem_id,sembuffer,number of sembuffer)
        */
        sem_buf.sem_op = -1;
        if(semop(sem_id,&sem_buf,1) == -1) {
            perror("Can't Acquire Semaphore");
            return 0;
        }
        #ifdef DEBUG_TIME
        perror("start reset shm!");
        #endif
        
        /* Realtime Data Acquire Finished */
        /* index 20 indicate php command sending status */
        *(ShmPTR + 20) = 'f';
        *(ShmPTR + 21) = 'f';
		#ifdef DEBUG_TIME
			printf("reset shared memory!");
		#endif
        
        /*
        ** Release the Semaphore
        */
        sem_buf.sem_op = 1;
        if(semop(sem_id,&sem_buf,1) == -1) {
            perror("Can't Acquire Semaphore");
            return 0;
        }
        #ifdef DEBUG_TIME
        perror("finish reset shm!");
        #endif
    } else if(rec_data_package[6] == 0xa0) {
		/* Store Voltage Monitor Data */
        strcat(query_statement,voltage_monitor_seg1);//(des,src)
        strcat(query_statement,user_id);
        strcat(query_statement,zoneId);
        strcat(query_statement,current);
        strcat(query_statement,voltage);
        strcat(query_statement,",");
        strcat(query_statement,"current_date,");
        strcat(query_statement,"current_time)");

        #ifdef DEBUG_TIME
            printf("%s\n", query_statement);
        #endif
        mysql_query(conn,query_statement);

	} else {
		printf("type is : %x\n",rec_data_package[6]);
	}
  	
	affected_rows = mysql_affected_rows(conn);

  	printf("Affected Row is:%ld\n",(long)affected_rows);

  	if((int)affected_rows < 0)perror("Insert action failed!");

  	mysql_close(conn);//close mysql connection
	
	//return 0;
}
