/* this program demonstrates the use of signal to trap interrupts  
 * from the terminal.  To terminate the program type ^\  
 */  
#include <signal.h>  
  
main()  
{ void cnt(int sig);  
 
  signal(SIGINT, cnt);  signal(SIGQUIT, SIG_IGN); 
      printf("Begin counting the INTERRUPTs\n");  
      for(;;);  /* infinite loop */  
}  
  
void cnt(int sig)  
{ static int count=0;    
      printf("Total of %d INTERRUPTS received\n", ++count);  
      /* if(count==3) signal(SIGINT, SIG_DFL); */
}  