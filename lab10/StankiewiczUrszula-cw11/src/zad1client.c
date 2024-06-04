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
struct sockaddr_in client_socket_addr;
struct pollfd client_fd;

void *at_exit()
{
    Message message;
    message.mtype = STOP;
    printf("\nExiting...\n");
    if (write(client_fd.fd, &message, sizeof(message)) < 0)
    {
        perror("write");
    }
    if (shutdown(client_fd.fd, 2) < 0)
    {
        perror("shutdown");
    }
    if (close(client_fd.fd) < 0)
    {
        perror("close");
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
        if (read(client_fd.fd, &message, MAX_CONTENT) < 0)
        {
            perror("read");
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
            struct tm *tm = localtime(&(message.time));
            char buff[BUFF_SIZE];
            strftime(buff, BUFF_SIZE, "%c", tm);
            printf("\nPrinting message from: %d sent on: %s; Content: %s\nIssue a command: ", message.from, buff, message.content);
            fflush(stdout);
            break;
        }
        case ONE:
        {
            struct tm *tm = localtime(&(message.time));
            char buff[BUFF_SIZE];
            strftime(buff, BUFF_SIZE, "%c", tm);
            printf("\nPrinting message from: %d sent on: %s; Content: %s\nIssue a command: ", message.from, buff, message.content);
            fflush(stdout);

            break;
        }
        case ALIVE:
        {
            printf("\nListing all active clients:\n%sIssue a command: ", message.content);
            fflush(stdout);

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

    printf("%d\n", client_addr.s_addr);

    in_port_t port;
    if (strcmp(argv[2], "NULL"))
    {
        port = 3333;
    }
    else if ((port = atoi(argv[2])) < 0)
    {
        perror("atoi");
        return;
    }
    port = atoi(argv[2]);

    client_socket_addr.sin_family = AF_INET;
    // client_socket_addr.sin_addr.s_addr=inet_addr(argv[1]);
    client_socket_addr.sin_addr = client_addr;
    client_socket_addr.sin_port = htons(port);

    printf("%d\n", port);

    if ((client_fd.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return;
    }
    if (connect(client_fd.fd, &client_socket_addr, sizeof(client_socket_addr)) < 0)
    {
        perror("connect");
        return;
    }

    atexit((void *)at_exit);
    signal(SIGINT, handle);

    Message message;
    message.mtype = NAME;
    strcpy(message.content, argv[3]);
    if (write(client_fd.fd, &message, sizeof(message)) < 0)
    {
        perror("write");
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
            if (write(client_fd.fd, &message, sizeof(message)) < 0)
            {
                perror("write");
                continue;
            }
        }
        else if (strcmp("stop", command) == 0)
        {
            printf("Sending STOP command to server...\n");
            Message message;
            message.mtype = STOP;
            if (write(client_fd.fd, &message, sizeof(message)) < 0)
            {
                perror("write");
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
            message.time = time(NULL);
            if (write(client_fd.fd, &message, sizeof(message)) < 0)
            {
                perror("write");
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
            message.time = time(NULL);
            message.to = to_fd;
            if (write(client_fd.fd, &message, sizeof(message)) < 0)
            {
                perror("write");
                continue;
            }
        }
        else if (strcmp("alive", command) == 0)
        {
            printf("Sending ALIVE command to server...\n");
            Message message;
            message.mtype = ALIVE;
            if (write(client_fd.fd, &message, sizeof(message)) < 0)
            {
                perror("write");
                continue;
            }
        }
        else if (strcmp("end_alive", command) == 0)
        {
            printf("Sending END_ALIVE command to server...\n");
            Message message;
            message.mtype = END_ALIVE;
            if (write(client_fd.fd, &message, sizeof(message)) < 0)
            {
                perror("write");
                continue;
            }
        }
        else
        {
            printf("Wrong command!\n");
        }
    }
}
