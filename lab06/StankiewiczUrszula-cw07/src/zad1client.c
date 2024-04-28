#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 2048
#define PRIORITY 2
#define INIT "INIT"
#define SERVER "/server"

struct QData{
    char charData[BUFF_SIZE];
    char id[BUFF_SIZE];
    
    int intData;
    
} ;

int main()
{   

    
    char buff[BUFF_SIZE];

    struct mq_attr attr;
    attr.mq_flags=0;
    attr.mq_maxmsg=10;
    attr.mq_msgsize=sizeof(struct QData);
    char q_name[BUFF_SIZE];

    sprintf(q_name,"/%d",getpid());

    // mqd_t mq=mq_open(q_name,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,&attr);

    // if(mq<0){
    //     perror("mq_open");
    //     return -1;
    // }

    struct QData send_data;
    memset(&send_data, 0, sizeof(struct QData));
    send_data.intData=1;
     
    memcpy((send_data.id), q_name,strlen(q_name)); 
    memcpy((send_data.charData), INIT,strlen(INIT));
    


    struct QData *aa=((struct QData *)((char *)&send_data));
    // printf("%s\n", send_data.charData);

    mqd_t server=mq_open(SERVER,O_RDWR,S_IRUSR|S_IWUSR,NULL);
    if(server<0){
        perror("mq_open");
    }
    char * msg=(char *)&send_data;
    printf("%d %s\n",sizeof(msg), (*((struct QData *) msg)).id);


    int a;

    if((a=mq_send(server,(char *)&send_data, sizeof(send_data),10))<0){
        perror("mq_send");
        return -1;
    }

    printf("%d %s\n",sizeof((char *)&send_data), (*((struct QData *) msg)).id);

    
    // if(mq_receive(mq,buff,sizeof(int),NULL)<0){
    //     perror("mq_receive");
    //     return -1;
    // }
    // printf("buff");

    // int id=atoi(buff);
    // int i=0;

    // pid_t child=fork();
    // if(child==0){
    //     while(i<5)
    //     {
    //         if(mq_receive(mq,buff,sizeof(struct QData),NULL)==-1){
    //             perror("mq_receive");
    //             return -1;
    //         }

    //         struct QData received_data;
    //         received_data=*((struct QData *) buff);

    //         printf("Received message: %s \n From: %d\n",received_data.charData,received_data.intData);

    //         i+=1;
    //     }

    // }else{
    //     while(i<5){
            
    //         int n=scanf("Write a message: %s\n",buff);
    //         if(n<0){
    //             perror("scanf");
    //             return -1;
    //         }

    //         if(mq_send(server,buff,n,PRIORITY)==-1){
    //             perror("mqsend");
    //             return -1;
    //         }
    //         i+=1;
    //     }
    // }

}