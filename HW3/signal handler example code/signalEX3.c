#include    <signal.h>
#include    <stdio.h>
static void sig_handler(int);

  int i, pid1, pid2, status;

int main () 
{

    signal(SIGTSTP, SIG_IGN) ; /* ignore SIGTSTP termination signal */


  if(signal(SIGINT, sig_handler) == SIG_ERR)
    printf("Parent: Unable to create handler for SIGINT\n");

  if(signal(SIGQUIT, sig_handler) == SIG_ERR)
    printf("Parent: Unable to create handler for SIGQUIT\n");

  pid1 = getpid(); printf("Parent pid = %d \n", pid1);
  if((pid2 = fork()) == 0){
     printf("Child pid = %d \n", getpid());
    for (;;) ;
  }
  else 
  {
    /*    for (;;) ; */
    wait(&status);
    printf("Parent: Status=%d\n", status);
    printf("Parent: done\n");
  }
}


static void sig_handler(int signo) {

  switch(signo) {
  case SIGINT:  /* Incoming SIGINT */
    printf("Process %d received SIGINT (Cntl-C)\n", getpid()); fflush(stdout);
    /* printf("Process %d: passing SIGINT to %d \n", getpid(), pid2); fflush(stdout); 
    /* kill(pid2, SIGINT); */
    break;

  case SIGQUIT:  /* Incoming SIGQUIT */

       signal(SIGTSTP, SIG_DFL) ; /* restore default action for SIGTSTP signal */

    printf("Process %d received SIGQUIT SIGQUIT SIGQUIT SIGQUIT \n", getpid());
    /* printf("Process %d: passing SIGQUIT SIGQUIT SIGQUIT to process %d ............ \n", getpid(), pid2);
    /*    kill(pid2, SIGQUIT); */
    break;
  default: break;
  }
  return;

}