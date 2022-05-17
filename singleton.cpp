#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <mutex>
using namespace std;
class Singleton
        {
                private:
                static Singleton* my_instance;
                Singleton();
                mutex mtx;

                public:
                static Singleton* Instance();
                void Destroy();
        };
Singleton* Singleton::my_instance = 0;
Singleton* Singleton::Instance()
{

    if (my_instance == 0)
    {
        my_instance = new Singleton();
    }

    return my_instance;
}

Singleton::Singleton()
{mtx.lock();}

void Singleton::Destroy(){
    my_instance=0;
    mtx.unlock();
}

int main()
{
    //new Singleton(); // Won't work
    Singleton* s = Singleton::Instance(); // Ok
    Singleton* r = Singleton::Instance();

    /* The addresses will be the same. */
    std::cout << s << std::endl;
    std::cout << r << std::endl;
}