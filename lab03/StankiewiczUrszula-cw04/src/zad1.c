#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argv[1] == NULL || argc < 1)
    {
        printf("Wrong number of arguments!\n");
        return 0;
    }

    int p_num = atoi(argv[1]);
    if (p_num == 0)
    {
        printf("Processes created: %d\n", p_num);
        exit(0);
    }
    pid_t child_pid = fork();

    for (int i = 1; i < p_num; i++)
    {
        if (child_pid != 0)
        {
            child_pid = fork();
        }
    }
    if (child_pid != 0)
    {
        for (int i = 0; i < p_num; i++)
        {
            wait(NULL);
        }
        printf("Processes created: %d\n", p_num);
    }
    else
    {
        printf("PID: %d, PPID: %d\n", (int)getpid(), (int)getppid());
    }
}