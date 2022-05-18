#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <mutex>
#include <fstream>
//
using namespace std;
template <typename T> class Singleton
        {
                private:
                static Singleton* my_instance;
                T my_t;
                Singleton(T temp);
                mutex mtx;

                public:
                static Singleton* Instance(T temp);
                void Destroy();
        };
template<typename T>
Singleton<T> * Singleton<T>::my_instance = 0;
template<typename T>
Singleton<T> *Singleton<T>::Instance(T temp) {

    if (my_instance == 0) {
        my_instance = new
        Singleton(temp);
    }

    return my_instance;
}
template<typename T>
Singleton<T>::Singleton(T temp) {
my_t = temp;
mtx.lock();
}
template<typename T>
void Singleton<T>::Destroy() {
    my_instance = 0;
    mtx.unlock();
}

int main() {
//    new Singleton(); // Won't work
    FILE *fptr;
//    cout << inputFile.fileDesc << endl;//made up call
    Singleton<FILE*>* s = Singleton<FILE*>::Instance(fptr); // Ok
    Singleton<FILE*>* r = Singleton<FILE*>::Instance(fptr);
//
//    /* The addresses will be the same. */
//    std::cout << s << std::endl;
//    std::cout << r << std::endl;
//    pthread_t t1;
//    pthread_t t2;
//    Singleton * s = pthread_create(&t1, NULL, Singleton::Instance, NULL);
//    Singleton * r = pthread_create(&t2, NULL, Singleton::Instance, NULL);
//    pthread_join(t1, NULL);
//    pthread_join(t2, NULL);
    std::cout << s << std::endl;
    std::cout << r << std::endl;
}
