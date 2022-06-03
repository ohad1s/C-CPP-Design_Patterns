// Credit:
//  https://www.geeksforgeeks.org/queue-linked-list-implementation/
// https://www.geeksforgeeks.org/condition-wait-signal-multi-threading/

// A C program to demonstrate linked list based implementation of queue
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

// Declaration of thread condition variable
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
// declaring mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
static pthread_t *client_thread_arr;
int where_am_i_client_arr = 0;

struct Queue *createQ();

static struct Queue *q;
static struct Queue *q2;
static struct Queue *q3;

// A linked list (LL) node to store a queue entry
struct QNode
{
    void *key;
    struct QNode *next;
    int sock_fd;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct Queue
{
    struct QNode *front, *rear;
};

// A utility function to create a new linked list node.
struct QNode *newNode(void *k)
{
    // pthread_mutex_lock(&lock);
    struct QNode *temp = (struct QNode *)malloc(sizeof(struct QNode));
    temp->key = (void *)malloc(sizeof(k));
    strcpy(temp->key, k);
    // temp->key = k;
    temp->next = NULL;
    // pthread_mutex_unlock(&lock);
    return temp;
}

// A utility function to create an empty queue
struct Queue *createQ()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a key k to q
void enQ(struct Queue *q, void *k)
{
    printf("enQ: %s\n", (char *)k);
    pthread_mutex_lock(&lock);
    // Create a new LL node
    struct QNode *temp = newNode(k);

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL)
    {

        q->front = q->rear = temp;
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&lock);
        return;
    }
    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;

    pthread_mutex_unlock(&lock);
}

void enQ2(struct Queue *q, void *k, int fd)
{
    //    printf("enQ: %s\n",(char*)k);
    pthread_mutex_lock(&lock);
    // Create a new LL node
    struct QNode *temp = newNode(k);
    temp->sock_fd = fd;

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL)
    {

        q->front = q->rear = temp;
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&lock);
        return;
    }
    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;

    pthread_mutex_unlock(&lock);
}

// Function to remove a key from given queue q
void *deQ(struct Queue *q)
{
    pthread_mutex_lock(&lock);
    if (q->front == NULL)
    {

        // let's wait on condition variable cond1
        printf("Waiting on condition variable cond1\n");
        pthread_cond_wait(&cond1, &lock);
    }
    // Store previous front and move front one node ahead
    struct QNode *temp = q->front;
    if (q->front)
    {
        q->front = q->front->next;
    }

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;
    pthread_mutex_unlock(&lock);
    return temp;
}

void destoryQ(struct Queue *q)
{
    while (q->front != NULL)
    {
        deQ(q);
    }
    free(q);
}

typedef struct active_object
{
    struct Queue *q;

    void *(*q_fun_ptr)(void *);

    void *(*f_fun_ptr)(void *);

    pthread_t my_pid;
} active_object;

void newAO(struct Queue *q, void *(*q_fun_ptr)(void *), void *(*f_fun_ptr)(void *))
{
    // print_queue(q);
    //    active_obj.my_pid = pthread_self();
    while (1)
    {
        struct QNode *n = (struct QNode *)deQ(q);
        q_fun_ptr(n);
        f_fun_ptr(n);
    }
}

void *newAO_th(void *args)
{
    active_object *ao = (active_object *)args;
    // print_queue(ao->q);
    newAO(ao->q, ao->q_fun_ptr, ao->f_fun_ptr);
}

void destroyAO(active_object *obj)
{
    printf("obj num: %lu die!!\n", obj->my_pid);
    destoryQ(obj->q);
    pthread_cancel(obj->my_pid);
    free(obj);
    printf("destroy AO finished!!\n");
}

typedef struct pipline
{
    active_object *first;
    active_object *second;
    active_object *third;
    active_object *fourth;
} pipline;

// ----------- server -------------------------------
void *get_msg(void *arg)
{
    int new_fd = *(int *)(arg);
    char *my_buffer = (char *)calloc(2000, 1);
    struct QNode *node_to_insert;
    int msg;
    int number_of_msg_to_client = 0;
    while (number_of_msg_to_client < 2) {
    msg = recv(new_fd, my_buffer, 2000, 0);
    if (msg == -1)
    {
        return;
    }
    else
    {
        printf("msg is %s. \n", my_buffer);
    }
    if (msg == 0)
    {
        return;
    }
    my_buffer[strlen(my_buffer)] = '\0';
    char *msg_str = (char *)calloc(1, strlen(my_buffer));
    strcpy(msg_str, my_buffer);
    //                node_to_insert = newNode(msg_str);
    //                node_to_insert->sock_fd = new_fd;
    //                printf("node to insert: %s \n", (char *) node_to_insert->key);
    enQ2(q, msg_str, new_fd);
    //                memset(e_ch, 0, 3);
    memset(my_buffer, 0, 2000);
    // number_of_msg_to_client++;
    // }
    }
    free(my_buffer);
}
void *play_server(void *qu)
{
    //    q = (struct Queue *) qu;
    //    ------- create socket -----------
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket : %d", sock);
        return NULL;
    }
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12000);
    if (bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("Bind sock %d failed\n", sock);
        close(sock);
        return NULL;
    }
    listen(sock, 10);
    // --------------------------
    printf("The server is ready you have 20sec\n");
    while (1)
    {
        struct sockaddr_storage their_addr;
        socklen_t sin_size = sizeof(their_addr);
        int new_fd = accept(sock, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd != -1)
        {
            pthread_t p;
            //            pthread_t thr;
            pthread_create(&p, NULL, get_msg, &new_fd);
            //            client_thread_arr[where_am_i_client_arr] = thr;
            //            where_am_i_client_arr++;
            //            printf("t num: %d \n", where_am_i_client_arr);
        }
    }
    return NULL;
}

// -----------------------------------------------------------------
void *fun1(void *arg)
{
    printf("Ohad and Dvir- fun1!\n");
    return NULL;
}

void *fun2(void *arg)
{
    printf("Ciiiiii -fun2!\n");
    return NULL;
}

void *ao1(void *arg)
{

    if (!arg)
    {
        return NULL;
    }
    struct QNode *n = (struct QNode *)arg;
    int len = strlen(n->key);
    char *str = malloc(sizeof(n->key));
    str = n->key;
    // printf("in func ao1: %s\n", str);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == 'z')
        {
            str[i] = 'a';
        }
        else if (str[i] == 'Z')
        {
            str[i] = 'A';
        }
        else
        {
            str[i] += 1;
        }
    }
    n->key = str;
    // printf("after func ao1: %s\n", (char*)n->key);
    return NULL;
}

void *ao2(void *arg)
{
    if (!arg)
    {
        return NULL;
    }
    struct QNode *n = (struct QNode *)arg;
    int len = strlen(n->key);
    char *str = malloc(sizeof(n->key));
    str = n->key;
    // printf("in func ao2: %s\n", str);
    for (int i = 0; i < len; i++)
    {
        if (65 <= str[i] && str[i] <= 90)
        {
            continue;
        }
        else
        {
            str[i] -= 32;
        }
    }
    n->key = str;
    // printf("after func ao2: %s\n", (char*)n->key);
    return NULL;
}

void *q_transpose1(void *arg)
{
    if (!arg)
    {
        return NULL;
    }

    struct QNode *n = (struct QNode *)arg;
    // printf("in func trans1: %s\n", (char*)n->key);
    int len = strlen(n->key);
    char str[len];
    strcpy(str, n->key);
    // printf("in func: %s\n", str);
    enQ2(q2, str, n->sock_fd);
}

void *q_transpose2(void *arg)
{
    if (!arg)
    {
        return NULL;
    }
    struct QNode *n = (struct QNode *)arg;
    // printf("in func trans2: %s\n", (char*)n->key);
    int len = strlen(n->key);
    char str[len];
    strcpy(str, n->key);
    // printf("in func: %s\n", str);
    // print_queue(q3);
    // printf("i === %d\n", i++);
    enQ2(q3, str, n->sock_fd);
}

void *print_node(void *arg)
{
    struct QNode *n = (struct QNode *)arg;
    printf("%s \n", (char *)n->key);
    return NULL;
}

void print_queue(struct Queue *q)
{
    if (!q)
    {
        return;
    }

    struct QNode *n = q->front;
    while (n != NULL)
    {
        printf("%s ,", (char *)n->key);
        n = n->next;
    }
    printf("\n");
}

void *msg_back(void *arg)
{
    printf("func msg back\n");
    struct QNode *n = (struct QNode *)arg;
    send(n->sock_fd, n->key, strlen((char *)n->key), 0);
    usleep(250);
    return NULL;
}

int main()
{
    q = createQ();
    q2 = createQ();
    q3 = createQ();

    //    active_object *obj = (active_object *) (malloc(sizeof(active_object)));
    active_object *obj2 = (active_object *)(malloc(sizeof(active_object)));
    active_object *obj3 = (active_object *)(malloc(sizeof(active_object)));
    active_object *obj4 = (active_object *)(malloc(sizeof(active_object)));
    //    obj->q = q;
    obj2->q = q;
    obj3->q = q2;
    obj4->q = q3;
    //    obj->q_fun_ptr = fun2;
    //    obj->f_fun_ptr = fun1;
    obj2->q_fun_ptr = ao1;
    obj2->f_fun_ptr = q_transpose1;
    obj3->q_fun_ptr = ao2;
    obj3->f_fun_ptr = q_transpose2;
    obj4->q_fun_ptr = msg_back;
    obj4->f_fun_ptr = print_node;
    pipline *pipline1 = (pipline *)(malloc(sizeof(pipline)));
    //    pipline1->first = obj;
    pipline1->first = NULL;
    pipline1->second = obj2;
    pipline1->third = obj3;
    pipline1->fourth = obj4;
    pthread_t server_t;
    pthread_create(&server_t, NULL, play_server, q);
    sleep(20);
    print_queue(q);
    //    pthread_t a_1, a_2, a_3, a_4;
    pthread_t a_2, a_3, a_4;
    //    pthread_create(&a_1, NULL, newAO_th, pipline1->first);
    //    usleep(200);
    pthread_create(&a_2, NULL, newAO_th, pipline1->second);
    sleep(6);
    pthread_create(&a_3, NULL, newAO_th, pipline1->third);
    sleep(6);
    pthread_create(&a_4, NULL, newAO_th, pipline1->fourth);
    sleep(6);
    //    printf("1:%lu, 2:%lu, 3:%lu, 4: %lu \n",a_1,a_2,a_3,a_4);
    //    pipline1->first->my_pid = a_1;
    pipline1->second->my_pid = a_2;
    pipline1->third->my_pid = a_3;
    pipline1->fourth->my_pid = a_4;
    //    printf("1:%lu, 2:%lu, 3:%lu, 4: %lu \n",pipline1->first->my_pid,pipline1->second->my_pid,pipline1->third->my_pid,pipline1->fourth->my_pid);

    //    destroyAO(pipline1->first);
    destroyAO(pipline1->second);
    destroyAO(pipline1->third);
    destroyAO(pipline1->fourth);
    close(sock);
    free(pipline1);
    return 0;
}