#include "3x3.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));
    Cube cube = create_random_cube();
    print_cube(&cube, true);
}