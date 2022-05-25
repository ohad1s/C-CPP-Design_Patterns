//Credit:
// https://www.geeksforgeeks.org/queue-linked-list-implementation/
//https://www.geeksforgeeks.org/condition-wait-signal-multi-threading/

// A C program to demonstrate linked list based implementation of queue
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <memory.h>
#include <netinet/in.h>

// Declaration of thread condition variable
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
// declaring mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// A linked list (LL) node to store a queue entry
struct QNode {
    void *key;
    struct QNode *next;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct Queue {
    struct QNode *front, *rear;
};

// A utility function to create a new linked list node.
struct QNode *newNode(void *k) {
    struct QNode *temp = (struct QNode *) malloc(sizeof(struct QNode));
    temp->key = k;
    temp->next = NULL;
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
void deQ(struct Queue *q) {
    pthread_mutex_lock(&lock);
    if (q->front == NULL) {

        // let's wait on condition variable cond1
        printf("Waiting on condition variable cond1\n");
        pthread_cond_wait(&cond1, &lock);
    }
    // Store previous front and move front one node ahead
    struct QNode *temp = q->front;

    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;

    free(temp);
    pthread_mutex_unlock(&lock);
}

void destoryQ(struct Queue *q) {
    while (q->front != NULL) {
        deQ(q);
    }
    free(q);
}

typedef struct active_object {
    struct Queue *q;

    void* (*q_fun_ptr)(void*);

    void* (*f_fun_ptr)(void*);

    pthread_t my_pid;
} active_object;

active_object newAO(struct Queue *q, void (*q_fun_ptr)(), void (*f_fun_ptr)()) {
    active_object active_obj;
    active_obj.f_fun_ptr = f_fun_ptr;
    active_obj.q_fun_ptr = q_fun_ptr;
    active_obj.q = q;
    struct QNode *n = q->front;
    while (n != NULL) {
        q_fun_ptr();
        n = n->next;
    }
    f_fun_ptr();
    return active_obj;
}

void destroyAO(active_object obj){
    destoryQ(obj.q);
//    pthread_cancel(obj.q_fun_ptr);
//    pthread_cancel(obj.f_fun_ptr);
    pthread_cancel(obj.my_pid);
    printf("destroy AO finished!!\n");
}


void fun1() {
    printf("Hell World!\n");
}

void fun2() {
    printf("Ciiiiii!\n");
}

void ao1(char *str){
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == 'z')
        {
            str[i] = 'a';
        }
        else if (str[i] =='Z')
        {
            str[i] = 'A';
        }
        else{
            str[i] += 1;
        }
    }
}
void ao2(char *str){
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        if (65 <= str[i] && str[i] <=90)
        {
            str[i] += 32;
        }
        else{
            str[i] -= 32;
        }

    }
}

int main() {
    struct Queue *q = createQ();

    pthread_t tid1, tid2;
    int x = 5;
    void *param[2] = {q, &x};
    // Create thread 1
    pthread_create(&tid1, NULL, deQ, q);

    // sleep for 1 sec so that thread 1
    // would get a chance to run first
    sleep(1);

    // Create thread 2
    pthread_create(&tid2, NULL, enQ, param);

    // wait for the completion of thread 2
    pthread_join(tid2, NULL);
//    printf("Queue Rear : %d\n", q->rear->key);
    enQ(q, 5);
    enQ(q, 10);
    enQ(q, 17);
    active_object obj = newAO(q, &fun1, &fun2);
    destroyAO(obj);



    return 0;
}