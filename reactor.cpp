
#include "reactor.hpp"

// typedef struct reactor
// {
//     int fd;
//     pthread_t id;
//     void*(*func)(void*);
// }reactor,*preactor;

// typedef struct reqests{
//     int fd;
//     preactor p_reactor;
// }reqests, *preqests;


preactor newReactor(){
    preactor r = (preactor)(malloc(sizeof(reactor)));
    return r;
}

void InstallHandler(preactor p_reactor, void*(func)(void*), int file_des){
    p_reactor->func = func;
    p_reactor->fd = file_des;
    preqests p_reqests = (preqests)(malloc(sizeof(reqests)));
    p_reqests->fd = file_des;
    p_reqests->p_reactor = p_reactor;
    pthread_create(&p_reactor->id, NULL, func, p_reqests);
}
void RemoveHandler(preactor p_reactor, int fd_free){
pthread_join(p_reactor->id, NULL);
p_reactor->fd = -1;
p_reactor->func = NULL;
}

