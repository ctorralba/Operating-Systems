#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/*
 * We're going to use the detailed signal handler
 */
void Handler( int signo, siginfo_t *si, void *context )
{
    int PID;
    switch( signo )
    {
        case SIGINT:
            printf("SIGINT Caught\n");
            break;
        case SIGQUIT:
            printf("SIGQUIT Caught\n");
            break;
            /*
             * This signal tells the parent that a child process did something
             */
        case SIGCHLD:
            PID = si->si_pid;
            /*
             * si_code is only specified for certain signals.  Use of this
             * field with other signals will give you garbage
             *
             * see sigaction man page for codes
             */
            switch(si->si_code)
            {
                case CLD_EXITED:
                    printf("Child exited\n");
                    break;
                 case CLD_KILLED:
                    printf("Child was killed\n");
                    break;
                 case CLD_STOPPED:
                    printf("Child was suspended\n");
                    break;
                 case CLD_CONTINUED:
                    printf("Child continued processing\n");
                    break;
            }

            break;
    }
}
        


int main( int argc, char *argv[], char *env[] )
{
    /*
     * the struct below is used to tell the OS whatsignal to handle
     * see sigaction() manual page
     */
    struct sigaction SA;
    char bob;
    int i;
    
    /*
     * Set the flags in the structure to force the OS to use the detailed
     * signal handler
     */
    SA.sa_flags |= SA_SIGINFO;

    /*
     * Set the handler in the structure
     */
    SA.sa_sigaction = Handler;

    /*
     * Since we don't want to save the old handler, we pass NULL as the third
     * argument
     *
     * The first argument is the signal we would like to handle
     */
    sigaction( SIGINT, &SA, NULL );
    sigaction( SIGQUIT, &SA, NULL );

    /*
     * When we get an interrupt it breaks out of the loop
     */
    while(fscanf( stdin, " %c ", &bob ) != EOF )
    {
        printf("Char: %c\n", bob );
    }

    return 0;
}