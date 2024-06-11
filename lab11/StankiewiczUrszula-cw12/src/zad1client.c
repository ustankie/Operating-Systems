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

struct in_addr client_addr;
struct sockaddr_in client_socket_addr, server_socket_addr;
struct pollfd client_fd;

void *at_exit()
{
    Message message;
    message.mtype = STOP;
    printf("\nExiting...\n");
    if (sendto(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
    {
        perror("sendto");
    }
}

void *handle()
{
    exit(0);
}

void *receive_messages(void *args)
{
    while (true)
    {
        Message message;
        int len = sizeof(server_socket_addr);
        if (recvfrom(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, &len) < 0)
        {
            perror("recvfrom");
            continue;
        }

        switch (message.mtype)
        {
        case LIST:
        {
            printf("\nListing all active clients:\n%sIssue a command: ", message.content);
            fflush(stdout);

            break;
        }
        case ALL:
        {
            printf("\nPrinting message from: %d sent on: %s; Content: %s\nIssue a command: ", message.from, message.time, message.content);
            fflush(stdout);
            break;
        }
        case ONE:
        {
            printf("\nPrinting message from: %d sent on: %s; Content: %s\nIssue a command: ", message.from, message.time, message.content);
            fflush(stdout);

            break;
        }
        case ALIVE:
        {
            printf("\nListing all active clients:\n%sIssue a command: ", message.content);
            fflush(stdout);

            break;
        }
        case PING:
        {
            // printf("Received message PING! Responding...\nIssue a command: ");
            // fflush(stdout);
            Message mess;
            mess.mtype = PING;
            if (sendto(client_fd.fd, &mess, sizeof(mess), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
            {
                perror("sendto");
            }
            break;
        }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        printf("Too little args\n");
        return;
    }
    if (strcmp(argv[1], "NULL") == 0)
    {
        client_addr.s_addr = INADDR_ANY;
    }
    else if (inet_aton(argv[1], &client_addr) == 0)
    {
        perror("inet_aton");
        return;
    }

    in_port_t port;
    in_port_t server_port;

    // if (strcmp(argv[2], "NULL"))
    // {
    //     port = 3333;
    // }
    // else if ((port = atoi(argv[2])) < 0)
    // {
    //     perror("atoi");
    //     return;
    // }

    if ((server_port = atoi(argv[2])) < 0)
    {
        perror("atoi");
        return;
    }

    client_socket_addr.sin_family = AF_INET;
    client_socket_addr.sin_addr = client_addr;
    client_socket_addr.sin_port = htons(0);
    printf("%d\n", client_addr.s_addr);

    server_socket_addr.sin_family = AF_INET;
    server_socket_addr.sin_addr = client_addr;
    server_socket_addr.sin_port = htons(server_port);
    printf("server_port: %d\n", server_port);

    if ((client_fd.fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        return;
    }

    atexit((void *)at_exit);
    signal(SIGINT, handle);

    if (bind(client_fd.fd, (struct sockaddr *)&client_socket_addr, sizeof(client_socket_addr)) < 0)
    {
        perror("bind");
        return;
    }

    Message message;
    message.mtype = NAME;
    strcpy(message.content, argv[3]);
    if (sendto(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
    {
        perror("sendto");
        exit(0);
    }
    printf("Sending NAME %s to server...\n", message.content);

    struct pollfd fds[1];
    fds[0] = client_fd;

    pthread_t thread;
    pthread_create(&thread, NULL, receive_messages, NULL);

    while (true)
    {
        printf("Issue a command: ");
        char buff[BUFF_SIZE];

        if (fgets(buff, BUFF_SIZE, stdin) == NULL)
        {
            perror("fgets");
            continue;
        }

        size_t len = strlen(buff);
        if (len > 0 && buff[len - 1] == '\n')
        {
            buff[len - 1] = '\0';
        }
        char *command = strtok(buff, " ");
        if (strcmp("list", command) == 0)
        {
            printf("Sending LIST command to server...\n");
            Message message;
            message.mtype = LIST;
            if (sendto(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
            {
                perror("sendto");
                continue;
            }
        }
        else if (strcmp("stop", command) == 0)
        {
            printf("Sending STOP command to server...\n");
            Message message;
            message.mtype = STOP;
            if (sendto(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
            {
                perror("sendto");
                continue;
            }

            exit(0);
        }
        else if (strcmp("2all", command) == 0)
        {
            char *arg = strtok(NULL, " ");
            if (arg == NULL)
            {
                printf("This command has one arg!\n");
                continue;
            }

            printf("Sending 2all %s command to server...\n", arg);

            Message message;
            message.mtype = ALL;
            strcpy(message.content, arg);
            time_t tim = time(NULL);
            struct tm *tm = localtime(&(tim));
            char buff[BUFF_SIZE];
            strftime(buff, BUFF_SIZE, "%c", tm);
            printf("Time: %s\n", buff);
            strcpy(&(message.time), buff);
            if (sendto(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
            {
                perror("sendto");
                continue;
            }
        }

        else if (strcmp("2one", command) == 0)
        {
            char *arg = strtok(NULL, " ");
            if (arg == NULL)
            {
                printf("This command has two args!\n");
                continue;
            }
            int to_fd = atoi(arg);

            arg = strtok(NULL, " ");
            if (arg == NULL)
            {
                printf("This command has two args!\n");
                continue;
            }

            printf("Sending 2one %d %s command to server...\n", to_fd, arg);

            Message message;
            message.mtype = ONE;
            strcpy(message.content, arg);
            time_t tim = time(NULL);
            struct tm *tm = localtime(&(tim));
            char buff[BUFF_SIZE];
            strftime(buff, BUFF_SIZE, "%c", tm);
            printf("Time: %s\n", buff);
            strcpy(&(message.time), buff);
            message.to = to_fd;
            if (sendto(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
            {
                perror("sendto");
                continue;
            }
        }
        else if (strcmp("alive", command) == 0)
        {
            printf("Sending ALIVE command to server...\n");
            Message message;
            message.mtype = ALIVE;
            if (sendto(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
            {
                perror("sendto");
                continue;
            }
        }
        else if (strcmp("end_alive", command) == 0)
        {
            printf("Sending END_ALIVE command to server...\n");
            Message message;
            message.mtype = END_ALIVE;
            if (sendto(client_fd.fd, &message, sizeof(message), 0, (struct sockaddr *)&server_socket_addr, sizeof(server_socket_addr)) < 0)
            {
                perror("sendto");
                continue;
            }
        }
        else
        {
            printf("Wrong command!\n");
        }
    }
}
