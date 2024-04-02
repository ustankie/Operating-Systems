#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int change_cnt = 0;
int curr_work_type = -1;

void handle(int signum, siginfo_t *info, void *context)
{
    int work_type = info->si_value.sival_int;

    printf("Received signal %d from %d, work type = %d\n", signum, info->si_pid, work_type);
    
    if (kill(info->si_pid, SIGUSR1) == -1)
    {
        perror("sigqueue");
        exit(EXIT_FAILURE);
    }

    switch (work_type)
    {
        case 1:
        {
            for (int i = 1; i <= 100; i++)
            {
                printf("%d ", i);
            }
            printf("\n");
            break;
        }
        case 2:
        {
            printf("Signal changes: %d\n", change_cnt);
            break;
        }
        case 3:
        {   
            printf("Exiting...\n");
            exit(EXIT_SUCCESS);
            break;
        }
        default:
        {
            printf("Wrong work type!\n");
            break;
        }
    }
    if (curr_work_type != work_type)
    {
        change_cnt++;
        curr_work_type = work_type;
    }
    printf("\n");
    
}

int main()
{
    struct sigaction sa;
    sa.sa_sigaction = handle;
    sigemptyset(&sa.sa_mask);

    printf("Catcher PID: %d\n", (int)getpid());
    sigaction(SIGUSR1, &sa, NULL);

    sigset_t waitmask;

    sigfillset(&waitmask);
    sigdelset(&waitmask, SIGUSR1);
    sigdelset(&waitmask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &waitmask, NULL) == -1)
    {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("\n");

    while (1)
    {
        sigsuspend(&waitmask);
    }

    printf("Catcher PID: %d\n", (int)getpid());
}