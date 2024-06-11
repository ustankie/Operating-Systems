#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include "config.h"
#include <pthread.h>

struct in_addr server_addr;
struct sockaddr_in server_socket_addr;
int server_fd;
bool alive_active = true;

struct pollfd client_fds[MAX_CLIENTS];
struct sockaddr_in client_addr[MAX_CLIENTS];
bool subscribers[MAX_CLIENTS];
char client_names[MAX_CLIENTS][MAX_CONTENT];

time_t ping_dates[MAX_CLIENTS];
time_t response_dates[MAX_CLIENTS];

void *ping(void *arg)
{
    Message message;
    message.mtype = ALIVE;
    while (true)
    {
        if (alive_active)
        {
            Message mess;
            mess.mtype = PING;
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_fds[i].fd != -1)
                {
                    int len = sizeof(client_addr[i]);
                    ping_dates[i] = time(NULL);
                    if (sendto(server_fd, &mess, sizeof(mess), 0, (struct sockaddr_in *)&(client_addr[i]), len) < 0)
                    {
                        perror("sendto");
                    }
                }
            }
        }
        sleep(5);
        bool first = true;

        for (int j = 0; j < MAX_CLIENTS; j++)
        {
            if ((client_fds[j].fd) != -1)
            {

                if ((response_dates[j] - ping_dates[j]) > 5 || response_dates[j] < ping_dates[j])
                {

                    if (first)
                    {
                        sprintf(message.content, "Client %d: No answer to ping - removing...\n", client_fds[j].fd);
                        first = false;
                    }
                    else
                    {
                        char buff[BUFF_SIZE];
                        sprintf(buff, "Client %d: No answer to ping - removing...\n", client_fds[j].fd);
                        strcat(message.content, buff);
                    }

                    printf("Client %d: No answer to ping - removing...\n", client_fds[j].fd);
                    client_fds[j].fd = -1;
                }
                else
                {

                    if (first)
                    {
                        sprintf(message.content, "Client %d alive\n", client_fds[j].fd);
                        first = false;
                    }
                    else
                    {
                        char buff[BUFF_SIZE];
                        sprintf(buff, "Client %d alive\n", client_fds[j].fd);
                        strcat(message.content, buff);
                    }

                    printf("Client %d alive\n", client_fds[j].fd);
                }
            }
        }
        for (int k = 0; k < MAX_CLIENTS; k++)
        {
            if (subscribers[k])
            {
                if (sendto(server_fd, &message, sizeof(message), 0, &(client_addr[k]), sizeof(client_addr[k])) < 0)
                {
                    perror("sendto");
                }
            }
        }
    }
}

void receive_messages()
{

    pthread_t alive_thread;
    pthread_t ping_thread;

    if (pthread_create(&ping_thread, NULL, ping, NULL) < 0)
    {
        perror("pthread");
    }

    while (true)
    {
        Message message;
        struct sockaddr_in from;
        memset(&from, 0, sizeof(struct sockaddr_in));
        from.sin_family = AF_INET;
        from.sin_addr.s_addr = htonl(INADDR_ANY);
        from.sin_port = htons((u_short)0);
        int len = sizeof(from);

        if (recvfrom(server_fd, &message, sizeof(message), 0, &from, &len) < 0)
        {
            perror("recvfrom");
            continue;
        }
        printf("%s\n", message.content);
        int curr_id = -1;
        for (int k = 0; k < MAX_CLIENTS; k++)
        {
            if (from.sin_addr.s_addr == (client_addr[k]).sin_addr.s_addr && from.sin_port == (client_addr[k]).sin_port)
            {
                curr_id = k;
                break;
            }
        }

        switch (message.mtype)
        {
        case LIST:
        {

            printf("Received message LIST from %d\n", curr_id);
            Message back_msg;
            bool first = true;
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if (client_fds[j].fd != -1)
                {
                    if (first)
                    {
                        sprintf(back_msg.content, "%d", j);
                        first = false;
                    }
                    else
                    {
                        char buff[BUFF_SIZE];
                        sprintf(buff, "%d", j);
                        strcat(back_msg.content, buff);
                    }
                    strcat(back_msg.content, "   ");
                    strcat(back_msg.content, client_names[j]);
                    strcat(back_msg.content, "\n");
                }
            }
            printf("%s", back_msg.content);
            back_msg.mtype = LIST;

            sendto(server_fd, &back_msg, sizeof(back_msg), 0, &(client_addr[curr_id]), sizeof(client_addr[curr_id]));
            break;
        }
        case STOP:
        {
            printf("Received message STOP from %d\n", curr_id);
            (client_fds[curr_id]).fd = -1;
        }
        case ALL:
        {
            printf("Received message ALL %s from %d\n", message.content, curr_id);
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if ((client_fds[j].fd) != -1 && (client_fds[j].fd != curr_id))
                {
                    printf("%d\n", j);
                    message.from = curr_id;
                    if (sendto(server_fd, &message, sizeof(message), 0, &(client_addr[j]), sizeof(client_addr[j])) < 0)
                    {
                        perror("sendto");
                    }
                }
            }
            break;
        }
        case ONE:
        {
            printf("Received message ONE %d %s from %d\n", message.to, message.content, curr_id);
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if ((client_fds[j].fd) == message.to)
                {
                    message.from = curr_id;
                    if (sendto(server_fd, &message, sizeof(message), 0, &(client_addr[j]), sizeof(client_addr[j])) < 0)
                    {
                        perror("sendto");
                    }
                }
            }
            break;
        }
        case ALIVE:
        {
            printf("Received message ALIVE from %d\n", curr_id);
            subscribers[curr_id] = true;
            break;
        }
        case END_ALIVE:
        {
            printf("Received message END_ALIVE from %d\n", curr_id);
            subscribers[curr_id] = false;
            break;
        }
        case NAME:
        {
            int i = get_free_client_space();
            if (i == -1)
            {
                printf("Unable to register, clientnum exceeded");
                continue;
            }

            client_addr[i] = from;
            client_fds[i].fd = i;

            printf("Received message type: %d from: %s\n", message.mtype, message.content);

            strcpy(client_names[i], message.content);
            printf("New client of fd: %d and name: %s\n", server_fd, client_names[i]);
            break;
        }
        case PING:
        {
            // printf("Received message PING from %d\n", curr_id);
            response_dates[curr_id] = time(NULL);
        }
        }
    }
}

int get_free_client_space()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if ((client_fds[i]).fd == -1)
        {
            return i;
        }
    }
    return -1;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Too little args\n");
        return;
    }
    if (strcmp(argv[1], "NULL") == 0)
    {
        server_addr.s_addr = INADDR_ANY;
    }
    else if (inet_aton(argv[1], &server_addr) == 0)
    {
        perror("inet_aton");
        return;
    }

    printf("Server started on address: %d\n", server_addr.s_addr);

    in_port_t port;
    if (strcmp(argv[2], "NULL") == 0)
    {
        port = 3333;
    }
    else
    {
        if ((port = atoi(argv[2])) < 0)
        {
            perror("atoi");
            return;
        }
    }
    server_socket_addr.sin_family = AF_INET;
    server_socket_addr.sin_addr = server_addr;
    server_socket_addr.sin_port = htons(port);

    printf("Port: %d\n", port);

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        return;
    }

    if (bind(server_fd, &server_socket_addr, sizeof(server_socket_addr)) < 0)
    {
        perror("bind");
        return;
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        (client_fds[i]).fd = -1;
        subscribers[i] = false;
    }

    receive_messages();
}
