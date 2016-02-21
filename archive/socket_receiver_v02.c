#include <stdio.h>
#include <stdlib.h>    //strlen
#include <unistd.h>    //write
#include <string.h>    //strlen strcat

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/tcp.h> //for macro in setsockopt
#include <arpa/inet.h> //inet_addr
 
#include <pthread.h> //for threading , link with lpthread

#include <mysql/my_global.h>
#include <mysql/mysql.h>

#define startByte 0xAA 
#define endByte 0x75
#define DEBUG_TIME

void *connection_handler(void *);
void restore_data(unsigned char *);//??
unsigned char* stringProcess(unsigned char *,int*);
 

/* Shared Memory Related Variable Declaration */
key_t mynotify;
int ShmID;
char *ShmPTR;

int main(int argc , char *argv[])
{   
    /* Socket Related Variable Declaretion */
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    //unsigned char *message;
    
	/* setsockopt related variable */
    int optval;

    //prepare shared memory
    mynotify = ftok("/usr/share/nginx/html/ipc/dan",'z');//generate a key
    ShmID = shmget(mynotify,100,0666);
    ShmPTR = (char *)shmat(ShmID,NULL,0);//attach a pointer to point at shared memory

    //Create socket 
	//socket_desc refers to "socket descriptor"
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);//parameter:ipv4 protocol,full-duplex stream socket,TCP protocol
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;//ipv4 protocol
    server.sin_addr.s_addr = inet_addr("121.43.109.2");//ip address(now this address is the address of my server )
    server.sin_port = htons(9988);//port number
     
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
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        //Reply to the client
        //message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
        //write(new_socket , message , strlen( message ));
         
        pthread_t sniffer_thread;//a unsigned int number,which used as thread identifier
        new_sock = malloc(1);
        *new_sock = new_socket;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
	prototype function to be executed by  the thread,
 	This will handle connection for each client.
*/
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int i;
    int sock = *(int*)socket_desc;
    int read_size;
    unsigned char client_message[256];
    unsigned char client_command[5] = {0xBB,0x30,0x70,0x00,0x00};
    unsigned char *temp;

    //message = (char *) malloc(256); 
    //Send some messages to the client
    //message = "Greetings! I am your connection handler\n";
    //write(sock , message , strlen(message));
     
    //message = "Now type something and i shall repeat what you type \n";
    //write(sock , message , strlen(message));//useless
     
    //Receive a message from client(Dead Loop)
    while( (read_size = recv(sock , client_message , 256 , 0)) > 0 )
    {
        //Send command back to client
        if(*ShmPTR == 'S') {
            #ifdef DEBUG_TIME
                printf("%s\n","Shared Memory Setted");
            #endif
			/* notification finished , so i got your command , and i am ready to execute it */
            *ShmPTR = 'F';
			/* now , we start wait reply of coordinator */
            *(ShmPTR + 2) = 'S';
            write(sock , client_command , 3);//send command to gprs
        }

        client_message[read_size] = '\0';
		
		temp = stringProcess(client_message,&read_size);
		if(read_size >= 8 && temp != NULL){
			restore_data(temp);
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
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
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

void restore_data(unsigned char *rec_data_package){ 

	MYSQL *conn;//connect handle(struct st_mysql)
  	my_ulonglong affected_rows;//unsigned __int64
	unsigned char user_id[10] = "";	
	unsigned char current[10] = "";
	unsigned char voltage[10] = "";
  	unsigned char query_statement_seg1[] = "INSERT INTO demo VALUES(";//change pointer on .data to array on heap
    unsigned char realtime_statement_seg1[] = "INSERT INTO RealTimeData VALUES(";
    unsigned char voltage_monitor_seg1[] = "INSERT INTO VoltageMonitor VALUES(";
	unsigned char query_statement[200] = "";
	unsigned int id,current_value,voltage_value;
	//validation
	if(*(rec_data_package) != startByte || *(rec_data_package + 7) != endByte){
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
	//printf("voltage value:%d\n",voltage_value);
	//*query_statement = "INSERT INTO GPRS_TEST (user_id,current_leak,voltage,rec_time) VALUES('0000','0.000','0.000','2000-01-01-00:00:00')";
	//construct query statement
	
	user_id[0] = 0x30 + id / 100;
	user_id[1] = 0x30 + (id % 100) / 10;
	user_id[2] = 0x30 + id % 10;
	user_id[3] = 0x2C;//comma
	user_id[4] = 0x00;//end of string

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
  	mysql_real_connect(conn, "localhost", "root", "382395","gprs", 0, NULL, 0);
	
    if(rec_data_package[6] == 0x01) {
		/* Store History Date */
        strcat(query_statement,query_statement_seg1);//(des,src)
        strcat(query_statement,user_id);
        strcat(query_statement,current);
        strcat(query_statement,voltage);
        strcat(query_statement,",");
        strcat(query_statement,"current_date,");
        strcat(query_statement,"current_time)");

        #ifdef DEBUG_TIME
            printf("%s\n", query_statement);
        #endif

        mysql_query(conn,query_statement);
    }
    else if(rec_data_package[6] == 0x50) {
		/* Store Realtime Data */
        strcat(query_statement,realtime_statement_seg1);//(des,src)
        strcat(query_statement,user_id);
        strcat(query_statement,current);
        strcat(query_statement,voltage);
        strcat(query_statement,")");

        #ifdef DEBUG_TIME
            printf("%s\n", query_statement);
        #endif

        mysql_query(conn,query_statement);
    }
    else if(rec_data_package[6] == 0x80) {
        /* Realtime Data Acquire Finished */
        *(ShmPTR + 1) = 'F';//reset shared memory
        *(ShmPTR + 2) = 'F';
		#ifdef DEBUG_TIME
			printf("%s\n","reset shared memory!");
		#endif
    }else if(rec_data_package[6] == 0xa0) {
		/* Store Voltage Monitor Data */
        strcat(query_statement,voltage_monitor_seg1);//(des,src)
        strcat(query_statement,user_id);
        strcat(query_statement,current);
        strcat(query_statement,voltage);
        strcat(query_statement,",");
        strcat(query_statement,"current_date,");
        strcat(query_statement,"current_time)");

        #ifdef DEBUG_TIME
            printf("%s\n", query_statement);
        #endif
        mysql_query(conn,query_statement);

	}else {
		printf("type is : %x\n",rec_data_package[6]);
	}
  	
	affected_rows = mysql_affected_rows(conn);

  	printf("Affected Row is:%ld\n",(long)affected_rows);

  	if(affected_rows < 0)printf("Insert action failed!\n");

  	mysql_close(conn);//close mysql connection
	
	//return 0;
}
