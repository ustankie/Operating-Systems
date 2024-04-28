#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
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
    // if (mq_unlink(SERVER) == -1) {
    //     perror("mq_unlink");
    //     exit(EXIT_FAILURE);
    // }
    struct mq_attr attr;
    attr.mq_flags=0;
    attr.mq_maxmsg=10;
    attr.mq_msgsize=sizeof(struct QData);
    mqd_t server=mq_open(SERVER,O_RDWR|O_CREAT, S_IRUSR|S_IWUSR, &attr);

    if(server<0){
        perror("mq_open");
        return 0;
    }

    pid_t clients[4];
    struct QData message;
    // memset(&message, 0, sizeof(struct QData));

    char buff[BUFF_SIZE];
    int a=0;
    if((a=mq_receive(server,(char *)&message,sizeof(struct QData),NULL))<0){
        perror("mq_receive");
        return -1;
    }
    // message=*((struct QData *)buff);

    printf("%d\n",sizeof(buff));


    printf("%d %s %s\n", message.intData, message.charData,message.id);



    
}