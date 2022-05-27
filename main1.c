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
struct QNode {
    void *key;
    struct QNode *next;
    int sock_fd;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct Queue {
    struct QNode *front, *rear;
};

// A utility function to create a new linked list node.
struct QNode *newNode(void *k) {
    // pthread_mutex_lock(&lock);
    struct QNode *temp = (struct QNode *) malloc(sizeof(struct QNode));
    temp->key = (void *) malloc(sizeof(k));
    strcpy(temp->key, k);
    // temp->key = k;
    temp->next = NULL;
    // pthread_mutex_unlock(&lock);
    return temp;
}

// A utility function to create an empty queue
struct Queue *createQ() {
    struct Queue *q = (struct Queue *) malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a key k to q
void enQ(struct Queue *q, void *k) {
    pthread_mutex_lock(&lock);
    // Create a new LL node
    struct QNode *temp = newNode(k);

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL) {

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
void *deQ(struct Queue *q) {
    pthread_mutex_lock(&lock);
    if (q->front == NULL) {

        // let's wait on condition variable cond1
        printf("Waiting on condition variable cond1\n");
        pthread_cond_wait(&cond1, &lock);
    }
    // Store previous front and move front one node ahead
    struct QNode *temp = q->front;
    if (q->front) {
        q->front = q->front->next;
    }

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;
    pthread_mutex_unlock(&lock);
    return temp;
}

void destoryQ(struct Queue *q) {
    while (q->front != NULL) {
        deQ(q);
    }
    free(q);
}

typedef struct active_object {
    struct Queue *q;

    void *(*q_fun_ptr)(void *);

    void *(*f_fun_ptr)(void *);

    pthread_t my_pid;
} active_object;

void newAO(struct Queue *q, void *(*q_fun_ptr)(void *), void *(*f_fun_ptr)(void *)) {
    // print_queue(q);
    //    active_obj.my_pid = pthread_self();
    while (1) {
        struct QNode *n = (struct QNode *) deQ(q);
        q_fun_ptr(n);
        // printf("in q : ");
        // print_queue(q2);
        f_fun_ptr(n);
        // printf("in q2 : ");
        // print_queue(q3);
    }
}

void *newAO_th(void *args) {
    active_object *ao = (active_object *) args;
    // print_queue(ao->q);
    newAO(ao->q, ao->q_fun_ptr, ao->f_fun_ptr);
}

void destroyAO(active_object* obj) {
    destoryQ(obj->q);
    pthread_cancel(obj->my_pid);
    free(obj);
    printf("destroy AO finished!!\n");
}

typedef struct pipline {
    active_object *first;
    active_object *second;
    active_object *third;
    active_object *fourth;
} pipline;

// -----------------------------------------------------------------

static struct Queue *my_q;

void *client_handle(void *varpg) {
    struct QNode *node_to_insert;
    char *my_buffer = (char *) calloc(2000, 1);
    int my_client_fd = ((int *) varpg)[0];
    int msg;
    char *e_ch = (char *) calloc(3, 1);
    int number_of_msg_to_client = 0;
    while (number_of_msg_to_client < 3) {
        msg = recv(my_client_fd, my_buffer, 2000, 0);
        if (msg == -1) {
            break;
        }
        strncpy(e_ch, my_buffer, 3);
        if (strcmp(e_ch, "DIE") == 0) {
            printf("client %d died after %d msgs..\n", my_client_fd, number_of_msg_to_client);
            close(my_client_fd);
            break;
        }
        if (msg == 0) {
            break;
        }
        my_buffer[strlen(my_buffer) - 1] = '\0';
        char *msg_str = (char *) calloc(1, strlen(my_buffer));
        strcpy(msg_str, my_buffer);
        node_to_insert = newNode(msg_str);
        node_to_insert->sock_fd = my_client_fd;
        enQ(q, node_to_insert);
        memset(e_ch, 0, 3);
        memset(my_buffer, 0, 2000);
        number_of_msg_to_client++;
    }

    free(my_buffer);
    free(e_ch);
    return NULL;
}

void *server_handle(void *varpg) {
    char *f = ((char *) varpg);
    char *e_ch = (char *) calloc(3, 1);
    char *input;
    while (1) {
        size_t my_cleaner = 0;
        if (getline(&input, &my_cleaner, stdin) == -1) {
            if (!feof(stdin)) { exit(1); }
        }
        strncpy(e_ch, input, 3);
        if (strcmp(e_ch, "DIE") == 0) {
            break;
        }
        if (strlen(input) != 0) // clean memory of str
        {
            free(input);
        }
        memset(e_ch, 0, 3);
    }
    free(e_ch);
    *f = '\0';
    return NULL;
}


void *play_server(void *qu) {
    q = (struct Queue *) qu;
//    ------- create socket -----------
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket : %d", sock);
        return NULL;
    }
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5000);
    if (bind(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        printf("Bind sock %d failed\n", sock);
        close(sock);
        return NULL;
    }
    listen(sock, 10);
// --------------------------
    printf("ready to serv^^..\n");
    pthread_t reciver_serv;
    char c = 'c';
    char *f = &c;
    pthread_create(&reciver_serv, NULL, server_handle, f);
    while (c != '\0') {
        struct sockaddr_storage their_addr;
        socklen_t sin_size = sizeof(their_addr);
        int new_fd = accept(sock, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd != -1) {
            pthread_t thr;
            pthread_create(&thr, NULL, client_handle, &new_fd);
            client_thread_arr[where_am_i_client_arr] = thr;
            where_am_i_client_arr++;
        }
    }
    int i = 0;
    pthread_t t = client_thread_arr[i];
    while (t) {
        pthread_join(t, NULL);
    }
    return NULL;
}

// -----------------------------------------------------------------
void* fun1(void* arg) {
    printf("Ohad and Dvir!\n");
    return NULL;
}

void fun2() {
    printf("Ciiiiii!\n");
}

void *ao1(void *arg) {
    if (!arg) {
        return NULL;
    }
    struct QNode *n = (struct QNode *) arg;
    int len = strlen(n->key);
    char *str = malloc(sizeof(n->key));
    str = n->key;
    // printf("in func ao1: %s\n", str);
    for (int i = 0; i < len; i++) {
        if (str[i] == 'z') {
            str[i] = 'a';
        } else if (str[i] == 'Z') {
            str[i] = 'A';
        } else {
            str[i] += 1;
        }
    }
    n->key = str;
    // printf("after func ao1: %s\n", (char*)n->key);
    return NULL;
}

void *ao2(void *arg) {
    if (!arg) {
        return NULL;
    }
    struct QNode *n = (struct QNode *) arg;
    int len = strlen(n->key);
    char *str = malloc(sizeof(n->key));
    str = n->key;
    // printf("in func ao2: %s\n", str);
    for (int i = 0; i < len; i++) {
        if (65 <= str[i] && str[i] <= 90) {
            str[i] += 32;
        } else {
            str[i] -= 32;
        }
    }
    n->key = str;
    // printf("after func ao2: %s\n", (char*)n->key);
    return NULL;
}

void *q_transpose1(void *arg) {
    if (!arg) {
        return NULL;
    }

    struct QNode *n = (struct QNode *) arg;
    // printf("in func trans1: %s\n", (char*)n->key);
    int len = strlen(n->key);
    char str[len];
    strcpy(str, n->key);
    // printf("in func: %s\n", str);
    enQ(q2, str);
}

void *q_transpose2(void *arg) {
    if (!arg) {
        return NULL;
    }
    struct QNode *n = (struct QNode *) arg;
    // printf("in func trans2: %s\n", (char*)n->key);
    int len = strlen(n->key);
    char str[len];
    strcpy(str, n->key);
    // printf("in func: %s\n", str);
    // print_queue(q3);
    // printf("i === %d\n", i++);
    enQ(q3, str);
}

void *print_node(void *arg) {
    struct QNode* n = (struct QNode*)arg;
    printf("%s ,", (char *) n->key);
    return NULL;
}

void print_queue(struct Queue *q) {
    if (!q) {
        return;
    }

    struct QNode *n = q->front;
    while (n != NULL) {
        printf("%s ,", (char *) n->key);
        n = n->next;
    }
    printf("\n");
}

void* msg_back(void* arg)
{
    struct QNode* n = (struct QNode* )arg;
    send(n->sock_fd, n->key, strlen((char*)n->key), 0);
    usleep(250);
    return NULL;
}

int main() {
    client_thread_arr = (pthread_t *) malloc(100 * sizeof(pthread_t));
    q = createQ();
    q2 = createQ();
    q3 = createQ();
    active_object *obj = (active_object *) (malloc(sizeof(active_object)));
    active_object *obj2 = (active_object *) (malloc(sizeof(active_object)));
    active_object *obj3 = (active_object *) (malloc(sizeof(active_object)));
    active_object *obj4 = (active_object *) (malloc(sizeof(active_object)));
    obj->q = q;
    obj2->q = q;
    obj3->q = q2;
    obj4->q=q3;
    obj->q_fun_ptr = play_server;
    obj->f_fun_ptr = fun1;
    obj2->q_fun_ptr = ao1;
    obj2->f_fun_ptr = q_transpose1;
    obj3->q_fun_ptr = ao2;
    obj3->f_fun_ptr = q_transpose2;
    obj4->q_fun_ptr=msg_back;
    obj4->f_fun_ptr=print_node;
    pipline *pipline1 = (pipline *) (malloc(sizeof(pipline)));
    pipline1->first = obj;
    pipline1->second = obj2;
    pipline1->third = obj3;
    pipline1->fourth - obj4;
    pthread_t a_1, a_2, a_3, a_4;
    pthread_create(&a_1, NULL, newAO_th, pipline1->first);
    usleep(200);
    pthread_create(&a_2, NULL, newAO_th, pipline1->second);
    usleep(200);
    pthread_create(&a_3, NULL, newAO_th, pipline1->third);
    usleep(200);
    pthread_create(&a_4, NULL, newAO_th, pipline1->fourth);

    pipline1->first->my_pid = a_1;
    pipline1->second->my_pid = a_2;
    pipline1->third->my_pid = a_3;
    pipline1->fourth->my_pid = a_4;

    sleep(200);
    destroyAO(pipline1->first);
    destroyAO(pipline1->second);
    destroyAO(pipline1->third);
    destroyAO(pipline1->fourth);


    close(sock);
    free(pipline1);
    return 0;
}