# Operating-Systems-Ex6

Ohad Shirazi
Dvir Biton

how to run:

make

now for each exercise you have another exe to run:

for 1-3 (thread safe queue, active object, pipline):
run ./main1 
then for each client run ./ao_client

for 4 (Guard):
./guard

for 5 (Singleton):
./singleton

for 6 (Reactor):
./pollServer
then for each client run ./clientReactor

## Testing:

* for 1-3 (thread safe queue, active object, pipline): run ./main1 and then for each client run ./test_ao_client
(after 20sec the first AO start to work so do it fast).
in the test we send from every client 2 words.
* for 4 (guard): run ./guard and wait, you will see the number '10' insert before '12' even though '10' in sleep 5 sec
* for 5 (singelton): run ./singleton and wait, you will see the same address in the singelton even though we insert two pointers.
* for 6 (singelton): run ./pollServer and then for each client run(min 2) run ./clientReactor, you will see just the others client get the message.
(Credit: all the credits are in the c/cpp files).