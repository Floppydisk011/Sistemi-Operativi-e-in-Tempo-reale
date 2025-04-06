#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL)); // time(NULL) -> current time in seconds

    int i = rand() % 100 + 1; // random number between 1 and 100

    printf("Random number: %d\n", i);
    return 0;
}
