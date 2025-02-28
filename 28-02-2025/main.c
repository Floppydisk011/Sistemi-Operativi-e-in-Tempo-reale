#include <stdio.h>
#include "fast_trigo.h"

int main() {
    float angle = 3.14 / 4;
    printf("sin(%f) = %f\n", angle, fast_sin(angle));
    printf("cos(%f) = %f\n", angle, fast_cos(angle));
    return 0;
}
