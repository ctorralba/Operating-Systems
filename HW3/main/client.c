//Christopher Torralba
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  /* define socket */ 
#include <netinet/in.h>  /* define internet socket */ 
#include <netdb.h>       /* define internet socket */ 
#include <pthread.h>     /* For multi-threading */
#include <signal.h>      /* For signals */
//changed server_port
#define SERVER_PORT 9912     /* define a server port number */ 

//To compile: gcc -o client client.c -lpthread -D_REENTRANT

void reader(void * arg);
void cnt(int sig);
char username[100];

int main(int argc, char* argv[]){
  int sd, k;
  pthread_t thread1;
  struct sockaddr_in server_addr = { AF_INET, htons(SERVER_PORT) };
  char buf[512];
  struct hostent *hp;
  char leaveBuf[50] = " has left the chatroom.";

  if (argc != 2) {
	  printf("Need a hostname\n");
	  exit(1);
  }

  //Getting the host
  if ((hp = gethostbyname(argv[1])) == NULL)
  {
	  printf("%s: %s unknown host\n", argv[0], argv[1]);
	  exit(1);
  }
  bcopy(hp->h_addr_list[0], (char*)&server_addr.sin_addr, hp->h_length);

  // creating a socket 
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
	  printf("client: socket failed");
	  perror("client: socket failed");
	  exit(1);
  }

  // connecting to the socket
  if (connect(sd, (struct sockaddr*)&server_addr,
	  sizeof(server_addr)) == -1)
  {
	  perror("client: connect FAILED:");
	  exit(1);
  }

  printf("Insert a username, then you can send messages:\n");

  //Welcome Message Handling
  if (gets(buf) != NULL){
  	memcpy(username, buf, 100);
	write(sd, buf, sizeof(buf));
	read(sd, buf, sizeof(buf));
	printf("%s\n", buf);
  }
  
  //Thread for creating messages
  pthread_create(&thread1, NULL, reader, sd);
  
  while (gets(buf) != NULL)
  {
  	  //sends the messages
  	  if ((strcmp(buf, "/exit") == 0) || (strcmp(buf, "/quit") == 0) || (strcmp(buf, "/part") == 0)){
  	  	//handle it then break
  	  	break;
	  }
	  signal(SIGINT, cnt); //^c
	  write(sd, buf, sizeof(buf)); //write whatever is on keyboard
  }
  
  //Message for leaving
  printf("%s has left the chatroom.\n", username);
  strcat(username, leaveBuf);
  write(sd, username, sizeof(username));
  
  //closing
  close(sd);
  return 0;
}

//pthread_create funciton
void reader(void* arg){
	int fd;
	char buf[512];
	char leaveBuf[50] = " has left the chatroom.";
	fd = (int*) arg;
	
	while( (read(fd, buf, sizeof(buf))) > 0) //while reading messages from the server
    {
    	//the server sends out a /quit to users
    	//if that is recieved then everybody quits
    	if (strcmp(buf, "/quit") == 0){
    	  printf("%s has left the chatroom.\n", username);
    	  //print out the user has left the server
  		  strcat(username, leaveBuf);
  		  write(fd, username, sizeof(username));
  		  //close connection
  		  close(fd);
  		  _exit(1);
		}
		else{
			//print regular messages recieved from server
			printf("%s\n", buf); 
		}
	    
    }
    close(fd);
}


void cnt(int sig){
	//if ctrl c was pressed
	printf("\nTo leave please /exit, /part, or /quit \n");
	return;
}
