#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int global = 0;

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        printf("Too few arguments!\n");
        return 0;
    }

    int catcher_pid = atoi(argv[1]);
    int work_type = atoi(argv[2]);


    union sigval work_type_union;
    work_type_union.sival_int = work_type;

    printf("Sender PID: %d\n", (int)getpid());

    if (sigqueue(catcher_pid, SIGUSR1, work_type_union) == -1)
    {
        perror("sigqueue");
        return 1;
    }

    sigset_t waitmask;

    sigfillset(&waitmask);
    sigdelset(&waitmask, SIGUSR1);
    sigdelset(&waitmask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &waitmask, NULL) == -1)
    {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    sigsuspend(&waitmask);

    return 0;
}