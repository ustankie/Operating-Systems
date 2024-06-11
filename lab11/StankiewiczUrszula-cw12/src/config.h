#include <time.h>
#define MAX_CONTENT 500
#define MAX_CLIENTS 4
#define BUFF_SIZE 2048

typedef enum
{
    LIST = 1,
    ALL = 2,
    ONE = 3,
    STOP = 4,
    ALIVE = 5,
    NAME = 6,
    END_ALIVE = 7,
    PING = 8

} MessageType;

typedef struct
{
    MessageType mtype;
    int from;
    int to;
    char content[MAX_CONTENT];
    char *list;
    char time[MAX_CONTENT];
} Message;
