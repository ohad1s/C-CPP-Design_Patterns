#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pid1, pid2, pid3;
    pid1 = fork();
    if (pid1 == 0) {
        sleep(1);
        system("./ao_client");
    } else {
        system("./main1");
    }
    return 1;
}


