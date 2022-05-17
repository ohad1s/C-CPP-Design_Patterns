.PHONY: all clean
CXX=gcc
CPP=g++

all: singleton #guard main1
main1: main1.o
	$(CXX) main1.o -o main1 -lpthread
main1.o: main1.c
	$(CXX) -c main1.c
guard: guard.o
	$(CPP) guard.o -o guard -lpthread
guard.o: guard.cpp
	$(CPP) -c guard.cpp
reactor: reactor.o
	$(CPP) reactor.o -o reactor
reactor.o: reactor.cpp
	$(CPP) -c reactor.cpp
singleton: singleton.o
	$(CPP) singleton.o -o singleton
singleton.o: singleton.cpp
	$(CPP) -c singleton.cpp
clean :
	rm -f *.o main1 guard reactor singleton