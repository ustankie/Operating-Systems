#include <stdio.h>
#include <signal.h>
#include <string.h>

void handler(int signum)
{
    printf("I got signal %d\n", signum);
}

int main(int argc, char *argv[])
{
    if (argv[1] == NULL || argc < 1)
    {
        printf("Wrong number of arguments!\n");
        return 0;
    }

    char *received_signal = argv[1];
    
    printf("Provided signal: %s\n", received_signal);
    if (strcmp(received_signal, "none") == 0)
    {
        signal(SIGUSR1, SIG_DFL);
    }
    else if (strcmp(received_signal, "ignore") == 0)
    {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(received_signal, "handler") == 0)
    {
        signal(SIGUSR1, handler);
    }
    else if (strcmp(received_signal, "mask") == 0)
    {   
        sigset_t newmask;
        sigset_t oldmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK,&newmask,&oldmask);

        
    }
    else
    {
        printf("Wrong arg!\n");
        return 1;
    }


    raise(SIGUSR1);


    if (strcmp(received_signal, "mask") == 0) {
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            printf("Signal SIGUSR1 is blocked.\n");
        } else {
            printf("Signal SIGUSR1 is not blocked.\n");
        }
    }
}