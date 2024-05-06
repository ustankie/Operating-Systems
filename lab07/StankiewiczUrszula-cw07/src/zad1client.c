#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <bits/waitflags.h>
#include <stdbool.h>
#include <string.h>

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
    printf("Got signal %d, signal: %d\n",getpid(), signum);
   
    
    if(parent==getpid()){
        kill(0,SIGINT);
        for (int i = 0; i < printers_num; i++)
        {
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

        shm_unlink(sh_name);

    }
    }
    exit(0);
    // if (parent!=NULL)
    // {
    //     kill(parent, SIGTERM);
    // }
}

void child_action(int i)
{
    srand(time(NULL)+i);
    while (!should_close)
    {
        printf("Client %d\n", i);
        char message[MESSAGE_SIZE];
        sleep(rand() % 5);

        for (int j = 0; j < MESSAGE_SIZE - 1; j++)
        {
            message[j] = 'a' + rand() % 26;
        }
        message[MESSAGE_SIZE - 1] = '\0';

        int valp = 0;

        int id=0;
        while(true){
            int a = sem_trywait(semaphores[id]);
            if(a==0){
                strcpy((addressess[id]->message), message);
                addressess[id]->client_num = i;

                printf("Client: %d, to print: %s\n", addressess[id]->client_num, (addressess[id]->message));
                break;
            }

            id=(id+1)%printers_num;        

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
        signal(sig, handle);
    }

    clients_num = atoi(argv[1]);
    printers_num = atoi(argv[2]);

    printf("Number of clients: %d\nNumber of printers: %d\n", clients_num, printers_num);

    for (int i = 0; i < printers_num; i++)
    {
        char sem_name[BUFF_SIZE];
        char sh_name[BUFF_SIZE];

        sprintf(sem_name, SEMAPHORE_PREFIX, i);
        sprintf(sh_name, SHARED_MEMORY_PREFIX, i);
        semaphores[i] = sem_open(sem_name, O_RDWR, S_IRUSR | S_IWUSR, 1);
        if (semaphores[i] == SEM_FAILED)
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

        shared_mem[i] = shm_open(sh_name, O_RDWR, S_IRUSR | S_IWUSR);
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


    if(setpgid(getpid(),getpid())<0){
        perror("setgid");
    }

    for (int i = 0; i < clients_num; i++)
    {   pid_t child = fork();
        if (child < 0)
        {
            perror("fork");
        }
        else if (child == 0)
        {
            printf("%d\n", getpid());
            child_action(i);
        }     
        
    }

    while (wait(NULL));

    for (int i = 0; i < printers_num; i++)
    {
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

        shm_unlink(sh_name);
    }
}