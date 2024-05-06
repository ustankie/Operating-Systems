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


#define SHARED_MEMORY_PREFIX "printer_sh%d"
#define SEMAPHORE_PREFIX "printer_sem%d"
#define BUFF_SIZE 2048
#define MESSAGE_SIZE 10

typedef struct{
    int client_num;
    char message[10];
} Data;

int clients_num;
int printers_num;
sem_t **semaphores;
int *shared_mem;
Data **addressess;

void handle(int signum){
    for(int i=0;i<printers_num;i++){

        if(sem_close(semaphores[i])==-1){
            perror("sem_close");
        }


        if(munmap(addressess[i],sizeof(Data))==-1){
            perror("munmap");
        }


        char sh_name[BUFF_SIZE];
        sprintf(sh_name, SHARED_MEMORY_PREFIX,i);

        if(shm_unlink(sh_name)==-1){
            perror("shm_unlink");
        }
    }


    free(semaphores);
    free(shared_mem);
 
    printf("Exiting...\n");

    pid_t child_pid;
    while ((child_pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        kill(child_pid, SIGTERM);
    }

    exit(0);

}


int main(int argc, char** argv){
    if(argc<3){
        printf("Too few arguments!\n");
        return -1;
    }

    for (int sig = 1; sig < SIGRTMAX; sig++) {
        signal(sig, handle);
    }

    clients_num=atoi(argv[1]);
    printers_num=atoi(argv[2]);

    printf("Number of clients: %d\nNumber of printers: %d\n",clients_num,printers_num);

    semaphores=malloc(sizeof(sem_t *)*printers_num);
    shared_mem=malloc(sizeof(int)*printers_num);
    addressess=malloc(sizeof(void *)*printers_num);

    for(int i=0;i<printers_num;i++){
        char sem_name[BUFF_SIZE];
        char sh_name[BUFF_SIZE];

        sprintf(sem_name, SEMAPHORE_PREFIX,i);
        sprintf(sh_name, SHARED_MEMORY_PREFIX,i);
        semaphores[i]=sem_open(sem_name,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR,1);
        int valp;
        sem_getvalue(semaphores[i], &valp);
        printf("%d\n", valp);

        shared_mem[i]=shm_open(sh_name,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);

        if(ftruncate(shared_mem[i],sizeof(Data))==-1){
            perror("ftruncate");
        }

        addressess[i]=mmap(NULL,sizeof(Data), PROT_READ| PROT_WRITE,MAP_SHARED, shared_mem[i],0);

    }
    pid_t parent=getpid();
    pid_t child=fork();


    if (child>0){
        for(int i=1;i<printers_num;i++){
            
            if(child>0){
                child=fork();
            }
        }
        if(child>0){
            for(int i=0;i<printers_num;i++){
                        wait(NULL);
                    }
        }
        
    }
    
    if (child==0){
        while(1){
            
        }
    }




    for(int i=0;i<printers_num;i++){

        if(sem_close(semaphores[i])==-1){
            perror("sem_close");
        }


        if(munmap(addressess[i],sizeof(Data))==-1){
            perror("munmap");
        }


        char sh_name[BUFF_SIZE];
        sprintf(sh_name, SHARED_MEMORY_PREFIX,i);

        if(shm_unlink(sh_name)==-1){
            perror("shm_unlink");
        }



    }


    free(semaphores);
    free(shared_mem);





}