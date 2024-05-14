#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h> 
#include <pthread.h> 
#include <unistd.h>
#include <stdbool.h>

#define REINDEERS_NUM 9
#define DELIVERY_NUM 4

typedef struct{
    int r_id;
}Args;

pthread_t r_threads[REINDEERS_NUM];

int waiting_num=0;
pthread_mutex_t waiting_num_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t all_ready_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond   = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_santa   = PTHREAD_COND_INITIALIZER;
bool santa_ready=false;

void increase_waiting(int i){
    pthread_mutex_lock(&waiting_num_mutex);

    waiting_num+=1;
    printf("Reindeer: %d reindeers are waiting for Santa, ID: %d\n",waiting_num, i);

    pthread_mutex_unlock(&waiting_num_mutex);
}

void set_zero_waiting(){
    pthread_mutex_lock(&waiting_num_mutex);

    waiting_num=0;

    pthread_mutex_unlock(&waiting_num_mutex);
}



void reindeer(void *arg){
    Args * args=(Args*) arg;
    srand(time(NULL)+args->r_id);

    printf("Reindeer %d started\n",args->r_id);

    while(true){
        sleep(rand()%6+5);
        increase_waiting(args->r_id);
        

        if(waiting_num==9){
            pthread_mutex_lock(&all_ready_mutex);
           

            set_zero_waiting();
            pthread_cond_broadcast(&cond);
            printf("Reindeer: All reindeers ready! ID: %d\n", args->r_id);
        
            pthread_mutex_unlock(&all_ready_mutex);   
        }else{
            pthread_mutex_lock(&all_ready_mutex);

            pthread_cond_wait(&cond, &all_ready_mutex);

            
            pthread_mutex_unlock(&all_ready_mutex);   
        }

        pthread_mutex_lock(&all_ready_mutex);
        pthread_cond_wait(&cond_santa, &all_ready_mutex);
        pthread_mutex_unlock(&all_ready_mutex);

        printf("Reindeer: Going for holidays, ID: %d\n", args->r_id);
    }
}

void *santa(){
    srand(time(NULL));
    for(int i=0;i<DELIVERY_NUM;i++){
        pthread_mutex_lock(&all_ready_mutex);
        santa_ready=false;

        pthread_cond_wait(&cond, &all_ready_mutex);
        

        pthread_mutex_unlock(&all_ready_mutex);  
     
        printf("Santa: I get up\n");
        printf("Santa: Delivering presents for the %d time...\n",i+1);

        sleep(rand()%3+2);
        pthread_mutex_lock(&all_ready_mutex);
        pthread_cond_broadcast(&cond_santa); 
        pthread_mutex_unlock(&all_ready_mutex);
        

    }

    for (int j = 0; j < REINDEERS_NUM; j++) {
        pthread_cancel(r_threads[j]);
    }
    return NULL;
}
int main(){

    for (int i = 0; i < REINDEERS_NUM; i++)
	{   
        Args *args=(Args *)malloc(sizeof(Args));
        args->r_id=i;


		if (pthread_create(&r_threads[i], NULL, (void*)&reindeer, (void*)args) != 0)
		{
			perror("pthread_create");
			return -1;
		}

		
	}

    pthread_t santa_thread;

    if(pthread_create(&santa_thread,NULL, &santa, NULL)!=0){
        perror("pthread_create");
        return -1;        
    }

    if(pthread_join(santa_thread,NULL)<0){
        perror("Thread join");
        return -1;
    }

    for (int i = 0; i < REINDEERS_NUM; i++){
        if(pthread_join(r_threads[i],NULL)<0){
            perror("Thread join");
            return -1;
        }
    }


}