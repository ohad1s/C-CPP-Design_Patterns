#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <arpa/inet.h>

#define SERVER_PORT 9034
#define SERVER_IP_ADDRESS "127.0.0.1"

int connect_flag = 0;
int sock = -1;

void *recvFunc(void *arg)
{
    char buff[1024] = {0};
    connect_flag = 1;
    int bytes = 0;
    while ((bytes = recv(sock, buff, 1024, 0)) != -1)
    {
        if (!bytes)
        {
            connect_flag = 0;
            break;
        }
        printf("recv: %s\n",buff);
        if (!strcmp(buff, "exit"))
        {
            connect_flag = 0;
            break;
        }
        bzero(buff, 1024);
    }
    return NULL;
}

void *sendFunc(void *arg)
{
    char input[1024] = {0};
    while (connect_flag != 0)
    {
        scanf("%s", input);
        printf("send: %s\n",input);
        if (strncmp(input,"exit",4) == 0)
        {
            send(sock,"exit",4,0);
            connect_flag = 0;
            break;
        }
        if (send(sock, input, strlen(input) + 1, 0) == -1)
        {
            perror("send");
        }
        bzero(input, 1024);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    printf("New client\n");
    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket");
        return -1;
    }
    signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket

    // open socket
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    // init client
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
    serverAddress.sin_port = htons(SERVER_PORT); // network order

    // connect to a server
    int clientSocket = connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (clientSocket == -1)
    {
        printf("listen failed");
        close(sock);
        return -1;
    }

    pthread_t pair_threads[2];
    connect_flag = 1;
    pthread_create(&pair_threads[0], NULL, recvFunc, NULL);
    pthread_create(&pair_threads[1], NULL, sendFunc, NULL);
    // pthread_join(pair_threads[0], NULL);
    pthread_join(pair_threads[1], NULL);
    pthread_kill(pair_threads[1], 0);

    close(sock);
    printf("The client send 'exit' and turn off\n");
    return 0;
}