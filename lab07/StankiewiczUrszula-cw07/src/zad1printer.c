#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <bits/waitflags.h>
#include <stdbool.h>

#define SHARED_MEMORY_PREFIX "printer_sh%d"
#define SEMAPHORE_PREFIX "printer_sem%d"
#define BUFF_SIZE 2048
#define MESSAGE_SIZE 11
#define MAX_PRINTERS 10
#define MAX_CLIENTS 10

volatile bool should_close = false;

typedef struct
{
    int client_num;
    char message[MESSAGE_SIZE];
} Data;

int clients_num;
int printers_num;
sem_t *semaphores[MAX_PRINTERS];
int shared_mem[MAX_PRINTERS];
Data *addressess[MAX_PRINTERS];
pid_t parent=NULL;

void handle(int signum)
{
    should_close = true;
    if(parent==getpid()){
        for (int i = 0; i < printers_num; i++)
    {
        // sem_destroy(semaphores[i]);
        if (sem_close(semaphores[i]) < 0)
        {
            perror("sem_close");
        }

        char sem_name[BUFF_SIZE];
        char sh_name[BUFF_SIZE];

        sprintf(sem_name, SEMAPHORE_PREFIX, i);
        sprintf(sh_name, SHARED_MEMORY_PREFIX, i);
        sem_unlink(sem_name);
        if (munmap(addressess[i], sizeof(Data)) == -1)
        {
            perror("munmap");
        }

        shm_unlink(sh_name) == -1;

    }}
    exit(0);
}

void printer_action(int i)
{
    printf("printer %d\n", i);
    while (!should_close)
    {
        int valp;
        if (sem_getvalue(semaphores[i], &valp) < 0)
        {
            perror("sem_getvalue");
        }

        if (valp == 0)
        {
            for (int j = 0; j < MESSAGE_SIZE - 1; j++)
            {
                printf("Client: %d, printer: %d, output: %c\n", addressess[i]->client_num, i, addressess[i]->message[j]);
                sleep(1);
            }
            if (sem_post(semaphores[i]) < 0)
            {
                perror("sem_post");
            }
        }
    }
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Too few arguments!\n");
        return -1;
    }

    for (int sig = 1; sig < SIGRTMAX; sig++)
    {
        signal(SIGINT, handle);
    }

    clients_num = atoi(argv[1]);
    printers_num = atoi(argv[2]);

    printf("Number of clients: %d\nNumber of printers: %d\n", clients_num, printers_num);

    for (int i = 0; i < printers_num; i++)
    {
        char sem_name[BUFF_SIZE];
        char sh_name[BUFF_SIZE];
        printf("in!\n");

        sprintf(sem_name, SEMAPHORE_PREFIX, i);
        sprintf(sh_name, SHARED_MEMORY_PREFIX, i);

        semaphores[i] = sem_open(sem_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 1);
        if (semaphores[i] < 0)
        {
            perror("sem_open");
            return -1;
        }
        int valp;
        if (sem_getvalue(semaphores[i], &valp) < 0)
        {
            perror("sem_get_value");
            return -1;
        }
        printf("valp: %d\n", valp);

        shared_mem[i] = shm_open(sh_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (shared_mem[i] < 0)
        {
            perror("shm_open");
            return -1;
        }

        if (ftruncate(shared_mem[i], sizeof(Data)) == -1)
        {
            perror("ftruncate");
        }

        addressess[i] = mmap(NULL, sizeof(Data), PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem[i], 0);
    }
    parent = getpid();
    pid_t child = -1;

    for (int i = 0; i < printers_num; i++)
    {
        printf("i: %d %d %d\n", i, getpid(), parent);
        pid_t child = fork();
        if (child < 0)
        {
            perror("fork");
            return -1;
        }
        else if (child == 0)
        {
            printf("%d\n", getpid());
            printer_action(i);
        }
    }

    while (wait(NULL) > 0)
        ;

    for (int i = 0; i < printers_num; i++)
    {
        // sem_destroy(semaphores[i]);
        if (sem_close(semaphores[i]) < 0)
        {
            perror("sem_close");
        }

        char sem_name[BUFF_SIZE];
        char sh_name[BUFF_SIZE];

        sprintf(sem_name, SEMAPHORE_PREFIX, i);
        sprintf(sh_name, SHARED_MEMORY_PREFIX, i);
        sem_unlink(sem_name);
        if (munmap(addressess[i], sizeof(Data)) == -1)
        {
            perror("munmap");
        }

        shm_unlink(sh_name) == -1;

    }
}