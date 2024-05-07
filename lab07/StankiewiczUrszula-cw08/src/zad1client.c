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
#define QUEUE_NAME "/pr_queue"
#define MAX_MSG 3

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
pid_t parent = NULL;
mqd_t pr_queue;

void handle(int signum)
{
    should_close = true;
    printf("Got signal %d from %d\n", signum, getpid());

    if (parent == getpid())
    {
        kill(0, SIGINT);
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

            mq_close(pr_queue);
        }
    }
    exit(0);

}

void child_action(int i)
{
    srand(time(NULL) + i);
    while (!should_close)
    {
        char message[MESSAGE_SIZE];
        sleep(rand() % 15 + 2);

        for (int j = 0; j < MESSAGE_SIZE - 1; j++)
        {
            message[j] = 'a' + rand() % 26;
        }
        message[MESSAGE_SIZE - 1] = '\0';

        int valp = 0;

        int id = 0;
        int success = false;
        while (id < printers_num)
        {
            int a = sem_trywait(semaphores[id]);
            if (a == 0)
            {
                strcpy((addressess[id]->message), message);
                addressess[id]->client_num = i;

                printf("Client: %d, to print: %s\n", addressess[id]->client_num, (addressess[id]->message));
                success = true;
                break;
            }

            id = (id + 1);
        }

        if (!success)
        {
            printf("Client %d: All printers are busy, sending msg %s to queue\n", i, message);
            Data data;
            memset(&data, 0, sizeof(Data));
            data.client_num = i;
            memcpy(data.message, message, strlen(message));
            struct mq_attr attr;

            mq_getattr(pr_queue, &attr);
            if (attr.mq_curmsgs >= attr.mq_maxmsg)
            {
                printf("Client %d: Queue full, please wait\n", i);
            }
            if (mq_send(pr_queue, (char *)&data, sizeof(data), 10) < 0)
            {
                perror("mq_send");
            }
            printf("Client %d: Message sucessfully sent to queue\n", i);
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
    struct mq_attr attr;
    attr.mq_curmsgs = 0;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = 2 * sizeof(Data);

    pr_queue = mq_open(QUEUE_NAME, O_RDWR, S_IWUSR | S_IRUSR, &attr);

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
        printf("Semaphore %d value: %d\n", i, valp);

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

    if (setpgid(getpid(), getpid()) < 0)
    {
        perror("setgid");
    }

    for (int i = 0; i < clients_num; i++)
    {
        pid_t child = fork();
        if (child < 0)
        {
            perror("fork");
        }
        else if (child == 0)
        {
            printf("New process %d\n", getpid());
            child_action(i);
        }
    }

    while (wait(NULL))
        ;

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