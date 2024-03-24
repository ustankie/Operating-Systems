#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[])
{

    if (argc < 1)
    {
        printf("Too few arguments!\n");
        return 0;
    }

    printf("%s\n", argv[0]);
    int local = 0;

    pid_t child_pid = fork();

    if (child_pid == -1)
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (child_pid == 0)
    {
        printf("Child process\n");
        local++;
        global++;
        printf("Child pid = %d, parent pid = %d\n", (int)getpid(), (int)getppid());
        printf("Child's local = %d, child's global = %d\n", local, global);
        int err = execl("/bin/ls", "ls", "-l", argv[1], NULL);
        printf("%d", err);
        exit(err);
    }
    else
    {
        int status;
        waitpid(child_pid, &status, 0);
        printf("Parent process\n");
        printf("Parent pid = %d, child pid = %d\n", (int)getpid(), (int)child_pid);
        printf("Child exit code: %d\n", status);
        printf("Parent's local = %d, parent's global = %d\n", local, global);
        return WEXITSTATUS(status);
    }
}