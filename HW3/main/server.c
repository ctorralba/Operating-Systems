//Christopher Torralba
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */
#include <pthread.h>     /* For multi-threading */
#include <string.h>
#include <signal.h>

#define SERVER_PORT 9912        /* define a server port number */
#define MAX_CLIENT 10

//To compile: gcc -o server server.c -lpthread -D_REENTRANT

int fileDesc[MAX_CLIENT] = { 0 };
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void message_func(void* arg);
void serverShutDown(int sig);

int main()
{
	pthread_t thread1;
    int sd,ns,k, username;
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
    struct sockaddr_in client_addr = { AF_INET };
    unsigned int client_len = sizeof( client_addr );
    char buf[512], *host;

    /* create a stream socket */
    if( ( sd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
	perror( "server: socket failed" );
	exit( 1 );
    }
    
    /* bind the socket to an internet port */
    if( bind(sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1 )
    {
	perror( "server: bind failed" );
	exit( 1 );
    }

    /* listen for clients */
    if( listen( sd, 10 ) == -1 )
    {
	perror( "server: listen failed" );
	exit( 1 );
    }

    printf("SERVER is listening for clients to establish a connection\n");
    
    //This is where the multithreading comes in accept returns a FD
    
    //while recieving new connections
    while( (ns = accept( sd, (struct sockaddr*)&client_addr,&client_len ) ) != -1 )
    {   
    	//lock
        pthread_mutex_lock(&mutex1);
        //set the new socket to the fileDescriptor array
        fileDesc[counter++] = ns;
        //unlock
        pthread_mutex_unlock(&mutex1);
        //multithread function
        pthread_create(&thread1, NULL, message_func, ns);
    }
	printf("end");
    
	close(ns);
    close(sd);
    unlink( (const char*)&server_addr.sin_addr);

    return(0);
}


//This only prints messages
void message_func(void* arg){ //ns int pointer
	int fd, k;
	char buf[512];
	char announceBuf[512] = "New user has joined, ";
	char userBuf[20] = "TBA";
	char writeBuf[512];
	char extraColon[3] = ": ";
	
	//if signal is pressed handle shutting down
	signal(SIGINT, &serverShutDown);	
	
	//set fd to the file descriptor socket
	fd = (int*) arg;
	
	//reading in the username
	if (k = read(fd, buf, sizeof(buf)) > 0){
		memcpy(userBuf, buf, 20);
	}
	else{
		printf("no username was specified");
	}
	
	//New user has joined, + username
	strcat(announceBuf, userBuf);
	
	//print message saying a new person joined
	printf("%s\n", announceBuf);
	//write this msg to all users 
	for (int i = 0; i < MAX_CLIENT; i++){
		if (fileDesc[i] != 0){
			write(fileDesc[i], announceBuf, sizeof(announceBuf));
		}
	}
	
	//username: 
	strcat(userBuf, extraColon);

	//get host name and readin
	while((k = read(fd, buf, sizeof(buf))) > 0){
		
		//add the username: to the message recieved
		memcpy(writeBuf, userBuf, 512);
		strcat(writeBuf, buf);
		
		//lock
		pthread_mutex_lock(&mutex1);
		printf("%s\n", writeBuf);
		
		//write the message to everyone but not themself
    	for (int i = 0; i < MAX_CLIENT; i++){
    		if (fileDesc[i] != 0 && fileDesc[i] != fd){
				write(fileDesc[i], writeBuf, sizeof(writeBuf));
    		}
		}
		//unlock
    	pthread_mutex_unlock(&mutex1);
    }
    
    //close and exit the thread afterwards (in main)
    for (int i = 0; i < MAX_CLIENT; i++){
    	if (fileDesc[i] == fd){
    		fileDesc[i] = 0;
    		break;
		}
	}

    close(fd);
}

//signal handling function
void serverShutDown(int sig){
	printf("\nServer is shutting itself down... \n");
	char shutBuf[100] = "Shutting the server down in 10 seconds... ";
	///quit is the message that is going to sent out to everyone
	char quitBuf[10] = "/quit";
	
	//send the shut down message
	for (int i = 0; i < MAX_CLIENT; i++){
		if (fileDesc[i] != 0){
			write(fileDesc[i], shutBuf, sizeof(shutBuf));
		}
	}
	//wait 10 seconds
	sleep(10);
	//send the /quit in the socket
	for (int i = 0; i < MAX_CLIENT; i++){
		if (fileDesc[i] != 0){
			write(fileDesc[i], quitBuf, sizeof(quitBuf));
		}
	}
	//have the users leave first
	sleep(2);
	//Get out of loop outside loop
	_exit(1);
	return;
}
