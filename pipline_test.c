#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pid1, pid2, pid3;
    pid1 = fork();
    if (pid1 == 0) {
        sleep(1);
        system("gnome-terminal --command=\"./ao_client\" ");
    } else {
        system("gnome-terminal --command=\"./main1\" ");
    }
    return 1;
}