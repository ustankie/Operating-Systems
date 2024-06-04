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

struct pollfd client_fds[MAX_CLIENTS];
struct sockaddr_in client_addr[MAX_CLIENTS];
char client_names[MAX_CLIENTS][MAX_CONTENT];
// pthread_mutex_t client_fds_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t receive_threads[MAX_CLIENTS];
typedef struct
{
    int fd;
    int i;
} thread_args;

void *at_exit()
{
    if (shutdown(server_fd, 2) < 0)
    {
        perror("shutdown");
    }
    if (close(server_fd) < 0)
    {
        perror("close");
    }
}

void *alive(void *args)
{
    thread_args t_args = *((thread_args *)args);
    int my_fd = t_args.fd;
    int i = t_args.i;

    struct pollfd fds[1];
    Message message;
    message.mtype = ALIVE;
    while (true)
    {
        bool first = true;

        for (int j = 0; j < MAX_CLIENTS; j++)
        {
            if ((client_fds[j].fd) != -1)
            {

                fds[0].fd = client_fds[j].fd;
                fds[0].events = POLLRDHUP;
                int ret = poll(fds, 1, 1000);
                if (ret < 0)
                {
                    perror("poll");
                    continue;
                }
                else if (ret != 0)
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
                    if (pthread_cancel(receive_threads[j]) < 0)
                    {
                        perror("pthread_cancel");
                    }
                    if (close(client_fds[j].fd) < 0)
                    {
                        perror("close");
                    }
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
            if (write(my_fd, &message, sizeof(message)) < 0)
            {
                perror("write");
            }
        }
        sleep(3);
    }
}

void *receive_messages(void *args)
{
    thread_args t_args = *((thread_args *)args);
    int my_fd = t_args.fd;
    int i = t_args.i;
    pthread_t alive_thread;

    while (true)
    {
        poll(client_fds, MAX_CLIENTS, 0);

        if (client_fds[i].fd == -1)
        {

            pthread_exit(0);
        }
        if (!(client_fds[i].revents & POLLIN))
        {
            continue;
        }
        Message message;
        if (read(my_fd, &message, MAX_CONTENT) < 0)
        {
            perror("read");
            continue;
        }

        switch (message.mtype)
        {
        case LIST:
        {
            printf("Received message LIST from %d\n", my_fd);
            Message back_msg;
            bool first = true;
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if (client_fds[j].fd != -1)
                {
                    if (first)
                    {
                        sprintf(back_msg.content, "%d", client_fds[j].fd);
                        first = false;
                    }
                    else
                    {
                        char buff[BUFF_SIZE];
                        sprintf(buff, "%d", client_fds[j].fd);
                        strcat(back_msg.content, buff);
                    }
                    strcat(back_msg.content, "   ");
                    strcat(back_msg.content, client_names[j]);
                    strcat(back_msg.content, "\n");
                }
            }
            printf("%s", back_msg.content);
            back_msg.mtype = LIST;

            write(my_fd, &back_msg, sizeof(back_msg));
            printf("Success\n");
            break;
        }
        case STOP:
        {
            printf("Received message STOP from %d\n", my_fd);
            (client_fds[i]).fd = -1;
            if (close(my_fd) < 0)
            {
                perror("close");
            }
            pthread_exit(0);
        }
        case ALL:
        {
            printf("Received message ALL %s from %d\n", message.content, my_fd);
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if ((client_fds[j].fd) != -1 && (client_fds[j].fd != my_fd))
                {
                    printf("%d\n", j);
                    message.from = my_fd;
                    if (write(client_fds[j].fd, &message, sizeof(message)) < 0)
                    {
                        perror("write");
                    }
                }
            }
            break;
        }
        case ONE:
        {
            printf("Received message ONE %d %s from %d\n", message.to, message.content, my_fd);
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if ((client_fds[j].fd) == message.to)
                {
                    message.from = my_fd;
                    if (write(client_fds[j].fd, &message, sizeof(message)) < 0)
                    {
                        perror("write");
                    }
                }
            }
            break;
        }
        case ALIVE:
        {
            printf("Received message ALIVE from %d\n", my_fd);
            pthread_create(&alive_thread, NULL, alive, (void *)&t_args);
            break;
        }
        case END_ALIVE:
        {
            printf("Received message END_ALIVE from %d\n", my_fd);
            pthread_cancel(alive_thread);
            break;
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

    printf("%d\n", server_addr.s_addr);

    in_port_t port;
    if (strcmp(argv[2], "NULL") == 0)
    {
        port = 0;
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
    // server_socket_addr.sin_addr=server_addr;
    // server_socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_socket_addr.sin_addr=server_addr;
    server_socket_addr.sin_port = htons(port);

    printf("%d\n", port);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return;
    }
    atexit((void *)at_exit);

    if (bind(server_fd, &server_socket_addr, sizeof(server_socket_addr)) < 0)
    {
        perror("bind");
        return;
    }

    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen");
        return;
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        (client_fds[i]).fd = -1;
    }

    while (true)
    {
        int i = get_free_client_space();
        if (i == -1)
        {
            printf("Unable to register, clientnum exceeded");
            continue;
        }
        int kliaddlen;

        kliaddlen = sizeof(struct sockaddr_in);

        int new_client_fd;
        if ((new_client_fd = accept(server_fd, &client_addr[i], &kliaddlen)) < 0)
        {
            perror("accept");
            continue;
        }

        Message message;
        if (read(new_client_fd, &message, MAX_CONTENT) < 0)
        {
            perror("read");
            continue;
        }

        printf("Received message type: %d from: %s\n", message.mtype, message.content);
        if (message.mtype != NAME)
        {
            printf("First message must be name!\n");
            continue;
        }

        client_fds[i].fd = new_client_fd;
        client_fds[i].events = POLLIN | POLLOUT;
        poll(client_fds, MAX_CLIENTS, 0);

        strcpy(client_names[i], message.content);
        printf("New client of fd: %d and name: %s\n", new_client_fd, client_names[i]);

        thread_args t_args;
        t_args.fd = new_client_fd;
        t_args.i = i;

        pthread_create(&(receive_threads[i]), NULL, receive_messages, (void *)&t_args);
    }
}
