/*
 * Signal Example with wait() and waitpid()
 *
 * Author: Brian Sea
 * Date: 9/17/03
 */

#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>

int i, pid1, pid2, status;

static void signal_handler(int signo)
{
    /* signo contains the signal number that was received */
    switch( signo )
    {
        /* Signal is a SIGINT */
        case SIGINT:
            printf( "Process %d: received SIGINT (Ctrl-C)\n", getpid() );
            fflush(stdout);
            printf( "Process %d: passing SIGINT to %d...\n", getpid(),pid2 );
            kill( pid2, SIGINT );
            break;
        
        /*  It's a SIGQUIT */
        case SIGQUIT:
            printf( "Process %d: received SIGQUIT (Ctrl-\\)\n", getpid() );
            fflush(stdout);
            printf( "Process %d: passing SIGQUIT to %d...\n", getpid(),pid2 );
            kill( pid2, SIGQUIT );
            break;

        default:
                break;
    }

    return;
}

int main( int argc, char *argv[], char *env[] )
{
    if( signal( SIGINT, signal_handler) == SIG_ERR  )
    {
        printf("Pærent: Unable to create handler for SIGINT\n");
    }

    if( signal( SIGQUIT, signal_handler) == SIG_ERR  )
    {
        printf("Pærent: Unable to create handler for SIGQUIT\n");
    }

    pid1 = getpid();
    printf( "Parent pid = %d\n", pid1 );

    if( (pid2 = fork()) == 0 )
    {
        /* Ignore the signals that are supposed to be received by the parent*/
        signal( SIGQUIT, SIG_IGN );

        /* Restore the default handler on SIGINT in the child */
        signal( SIGINT, SIG_DFL );

        printf( "Child pid = %d\n", getpid() );
        for( ;; )
        {
            sleep( 1 );
        }
    }
    else
    {
        /* 
         * This waits for ANY child to die.  It doesn't matter if the child
         * dies normally or from a signal.  The satus information is then
         * stored in the status integer.
         */
        wait( &status );
        
        /*
         * The information in status is *NOT* the return code!!  To make use
         * of the information we must macros to extract the needed
         * information.
         */

        /* WIFEXITED() determines if the process exitec normally (returned a
         * number).  This can be done through a return or exit()
         */

        if( WIFEXITED( status ) )
        {
            /* 
             * Now we know the process exited properly so we can get the
             * return value
             */

            printf( "Child return - %d\n", WEXITSTATUS( status ) );
        }
        else
        {
            /* Well it didn't exited properly.  Was it a signal? */
            if( WIFSIGNALED( status ) )
            {
                /* 
                 * Yes. A signal killed the child process.  Now we can extract
                 * the signal information from status
                 */
                
                printf( "Child died on signal - %d\n", WTERMSIG( status ));
            }
        }

        /* 
         * There are two other macros most UNIXes use.  They are:
         *  WIFSTOPPED() and WSTOPSIG().  See the man pages on the dells for
         *  more information.
         *
         *  To wait on a particular pid - see waitpid()
         */
    }

    return 0;
}