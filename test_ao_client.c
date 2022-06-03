
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
#include <time.h>
#include <stdlib.h>

static int sock1 = -1;

void *receive(void *arg) {
    char *my_buffer = (char *) calloc(2000, 1);

    int bytes = 0;
    while ((bytes = recv(sock1, my_buffer, 2000, 0)) != -1) {
        if (bytes == 0) {
            break;
        } else {
            printf("I got back the string: %s\n", my_buffer);
        }
        bzero(my_buffer,2000);
    }
    free(my_buffer);
    return NULL;
}


void *my_send(void *arg) {
    char *a = "OhaD";
    char *b = "dVir";
    char *c = "yoSSi";
    char *d = "Maccabi";
    char *e = "Haifa";
    char *f = "Omer";
    char *g = "Haziza";
    char *h = "HelloWorld";
    char *i = "WOW";
    for (int j=0; j<2; j++){
        srand(time(NULL));
        int r = rand()%8;
        if (r==0){
            printf("send: %s \n", a);
            send(sock1, a, strlen(a), 0);
            sleep(1);
        }
        if (r==1){
            printf("send: %s \n", b);
            send(sock1, b, strlen(b), 0);
            sleep(1);
        }
        if (r==2){
            printf("send: %s \n", c);
            send(sock1, c, strlen(c), 0);
            sleep(1);
        }
        if (r==3){
            printf("send: %s \n", d);
            send(sock1, d, strlen(d), 0);
            sleep(1);
        }
        if (r==4){
            printf("send: %s \n", e);
            send(sock1, e, strlen(e), 0);
            sleep(1);
        }
        if (r==5){
            printf("send: %s \n", f);
            send(sock1, f, strlen(f), 0);
            sleep(1);
        }
        if (r==6){
            printf("send: %s \n", g);
            send(sock1, g, strlen(g), 0);
            sleep(1);
        }
        if (r==7){
            printf("send: %s \n", h);
            send(sock1, h, strlen(h), 0);
            sleep(1);
        }
        if (r==8){
            printf("send: %s \n", i);
            send(sock1, i, strlen(i), 0);
            sleep(1);
        }
    }
    return NULL;
}

int main() {
//------ open socket
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 == -1) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    // init client
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(12000);  //network order
    printf("Client is ready\n");
    int clientSocket = connect(sock1, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (clientSocket == -1) {
        perror("socket");
        printf("listen failed");
        close(sock);
        return -1;
    }
    printf("successfully logged in\n");
//------------

    pthread_t t1, t2;

    // exec rcv, send operations threads
    pthread_create(&t1, NULL, receive, NULL);
    pthread_create(&t2, NULL, my_send, NULL);
    printf("cii 1\n");
    // join both threads
    pthread_join(t2, NULL);
    printf("cii 2\n");
    pthread_join(t1, NULL);
    printf("cii 3\n");

    close(sock); // close socket
    return 0;
}