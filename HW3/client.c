/************************************************************************/ 
/*   PROGRAM NAME: client.c  (works with serverX.c)                     */ 
/*                                                                      */ 
/*   Client creates a socket to connect to Server.                      */ 
/*   When the communication established, Client writes data to server   */ 
/*   and echoes the response from Server.                               */ 
/*                                                                      */ 
/*   To run this program, first compile the server_ex.c and run it      */ 
/*   on a server machine. Then run the client program on another        */ 
/*   machine.                                                           */ 
/*                                                                      */ 
/*   COMPILE:    gcc -o client client.c -lnsl                           */ 
/*   TO RUN:     client  server-machine-name                            */ 
/*                                                                      */ 
/************************************************************************/ 
 
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  /* define socket */ 
#include <netinet/in.h>  /* define internet socket */ 
#include <netdb.h>       /* define internet socket */ 
 //changed server_port
#define SERVER_PORT 9901     /* define a server port number */ 
 
int main( int argc, char* argv[] ) 
{ 
    int sd; 
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) }; 
    char buf[512]; 
    struct hostent *hp; 
 
    if( argc != 2 ) 
    { 
    //This is in use...
	printf( "Usage: %s hostname\n", argv[0] ); 
	exit(1); 
    } 
 
    /* get the host */ 
    if( ( hp = gethostbyname( argv[1] ) ) == NULL ) 
    { 
	printf( "%s: %s unknown host\n", argv[0], argv[1] ); 
	exit( 1 ); 
    } 
    bcopy( hp->h_addr_list[0], (char*)&server_addr.sin_addr, hp->h_length ); 
    printf("test");
 	/*for (int i = 0; i < hp->h_length; i++){
 		printf("%d\n", hp->h_addr_list[i]);
	 }*/
 
    /* create a socket */ 
    if( ( sd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) 
    { 
	perror( "client: socket failed" ); 
	exit( 1 ); 
    } 
 
    /* connect a socket */ 
    if( connect( sd, (struct sockaddr*)&server_addr, 
		 sizeof(server_addr) ) == -1 ) 
    { 
	perror( "client: connect FAILED:" ); 
	exit( 1 ); 
    } 
 
    printf("connect() successful! will send a message to server\n"); 
    
    
    //write(sd, buf, sizeof(buf));
    printf("Input a string:\n" ); 
 
 	while (1){
 		//first readin then write
 		if ((k = read(sd, buf, sizeof(buf))) > 0){
     		printf("SERVER ECHOED: %s\n", buf); 
     	}
     	if (gets(buf) != NULL){
			write(sd, buf, sizeof(buf)); 
		}
     	
	}
 	/*
 	while( gets(buf) != NULL) 
    { 
     write(sd, buf, sizeof(buf)); 
     read(sd, buf, sizeof(buf)); 
     printf("SERVER ECHOED: %s\n", buf); 
    }*/

    close(sd);
    return(0); 
} 
