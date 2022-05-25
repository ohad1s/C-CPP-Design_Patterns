#pragma once
#include <iostream>
#include <pthread.h>


typedef struct reactor
{
    int fd;
    pthread_t id;
    void*(*func)(void*);
}reactor,*preactor;

typedef struct reqests{
    int fd;
    preactor p_reactor;
}reqests, *preqests;

preactor newReactor();
void InstallHandler(preactor p_reactor, void*(func)(void*), int file_des);
void RemoveHandler(preactor p_reactor, int fd_free);
