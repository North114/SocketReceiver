#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
 
#include <pthread.h> //for threading , link with lpthread
 
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    int optval;
    unsigned int optlen = sizeof(optval);
    //char *message;
     
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
    server.sin_port = htons( 9988 );//port number
    
    /* set keepalive option */
    optval = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));//open keep-alive
    optval = 5;
    setsockopt(socket_desc, SOL_TCP,TCP_KEEPCNT, &optval, sizeof(int));//number of probe
    optval = 30;
    setsockopt(socket_desc, SOL_TCP, TCP_KEEPIDLE, &optval, sizeof(int));//keep-alive time(second)
    optval = 5;
    setsockopt(socket_desc, SOL_TCP, TCP_KEEPINTVL, &optval, sizeof(int));//probe interval
    
    /* Check the status for the keepalive option */
    if(getsockopt(socket_desc, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
	perror("getsockopt() error!\n");
	close(socket_desc);
	exit(EXIT_FAILURE);
    }
    printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

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
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        //Reply to the client
        //message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
        //write(new_socket , message , strlen(message));
         
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
    unsigned char client_message[1024];
     
    //Send some messages to the client
    //message = "Greetings! I am your connection handler\n";
    //write(sock , message , strlen(message));
     
    //message = "Now type something and i shall repeat what you type \n";
    //write(sock , message , strlen(message));
     
    //Receive a message from client , once connect failed , keepalive machanism will close make the return of recv function to -1
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        //write(sock , client_message , strlen(client_message));
		client_message[read_size] = '\0';
		for(i = 0;i < read_size;++i) {
			printf("%x ",*(client_message + i));
		}
		printf("\n");
		read_size = 0;
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");//print error reason after "recv failed"
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}
