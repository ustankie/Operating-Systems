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
#include <mqueue.h>

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
    if (parent == getpid())
    {
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

            shm_unlink(sh_name) == -1;

            mq_close(pr_queue);
            mq_unlink(QUEUE_NAME);
        }
    }
    exit(0);
}

void print_message(Data *data, int i, int is_from_queue)
{

    for (int j = 0; j < MESSAGE_SIZE - 1; j++)
    {
        if (is_from_queue)
        {
            printf("Client: %d from queue, printer: %d, output: %c\n", data->client_num, i, data->message[j]);
        }
        else
        {
            printf("Client: %d, printer: %d, output: %c\n", data->client_num, i, data->message[j]);
        }
        sleep(1);
    }
    if (sem_post(semaphores[i]) < 0)
    {
        perror("sem_post");
    }
}

void printer_action(int i)
{
    while (!should_close)
    {
        int valp;
        if (sem_getvalue(semaphores[i], &valp) < 0)
        {
            perror("sem_getvalue");
        }

        if (valp == 0)
        {
            print_message(addressess[i], i, false);
        }
        else
        {
            Data msg;
            memset(&msg, 0, sizeof(Data));
            struct mq_attr attr;
            mq_getattr(pr_queue, &attr);
            if (attr.mq_curmsgs > 0)
            {
                int a;
                if ((a = mq_receive(pr_queue, (char *)&msg, 2 * sizeof(Data), NULL)) >= 0)
                {
                    sem_wait(semaphores[i]);
                    print_message(&msg, i, true);
                }
                if (a < 0)
                {
                    perror("mq_receive");
                }
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

    struct mq_attr attr;
    attr.mq_curmsgs = 0;
    attr.mq_flags = O_NONBLOCK;
    attr.mq_maxmsg = MAX_MSG;
    attr.mq_msgsize = 2 * sizeof(Data);

    pr_queue = mq_open(QUEUE_NAME, O_RDONLY | O_CREAT, S_IWUSR | S_IRUSR, &attr);

    for (int i = 0; i < printers_num; i++)
    {
        char sem_name[BUFF_SIZE];
        char sh_name[BUFF_SIZE];

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
        printf("Semphore %d value: %d\n", i, valp);

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
        pid_t child = fork();
        if (child < 0)
        {
            perror("fork");
            return -1;
        }
        else if (child == 0)
        {
            printf("New process %d\n", getpid());
            printer_action(i);
        }
    }

    while (wait(NULL) > 0)
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

        shm_unlink(sh_name) == -1;
    }
}