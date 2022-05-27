#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <memory.h>
#include <netinet/in.h>
#include <sys/types.h>
#include "sock.h"
static int connect_flag = 0;
// DIE - in order to close socket
void* receive(void* arg)
{
    char *my_buffer = (char*)calloc(2000, 1);
    connect_flag = 1;
    int bytes = 0;
    while ((bytes = recv(sock, my_buffer, 2000, 0)) != -1 )
    {
        if (bytes == 0)
        {
            connect_flag = 0;
            break;
        }
        if (!strcmp(my_buffer, "DIE"))
        {
            connect_flag = 0;
            break;
        }
        memset(my_buffer, 0, 2000);
    }
    free(my_buffer);
    return NULL;
}


void* my_send(void* arg)
{
    char str1[80];
    char str2[80];
    char str3[80];
    printf("Enter your string: ");
    scanf("%s", str1);
    printf("Your string is %s.", str1);
    send(sock, str1, strlen(str1), 0);
    sleep(1);

    printf("Enter your string: ");
    scanf("%s", str2);
    printf("Your string is %s.", str2);
    send(sock, str2, strlen(str2), 0);
    sleep(1);

    printf("Enter your string: ");
    scanf("%s", str3);
    printf("Your string is %s.", str3);
    send(sock, str3, strlen(str3), 0);
    sleep(1);

    sleep(120);
    connect_flag = 0;
    return NULL;
}

int main()
{
//------ open socket
    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 == -1) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    // init client
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(5000);  //network order
    printf("client sock is on ^^\n");
sleep(10);
    int clientSocket = connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (clientSocket == -1)
    {
        printf("listen failed");
        close(sock);
        return -1;
    }
    printf("successfully logged in\n");
//------------

    pthread_t t1,t2;

    connect_flag = 1;
    // exec rcv, send operations threads
    pthread_create(&t1, NULL, receive, NULL);
    pthread_create(&t2, NULL, my_send, NULL);

    // join both threads
    pthread_join(t2, NULL);
    pthread_kill(t1, 0);

    close(sock); // close socket
    return 0;
}