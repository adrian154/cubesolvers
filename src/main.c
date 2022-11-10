#include "search.h"
#include "coordinates.h"
#include "3x3.h"
#include <stdio.h>

void print_solution(int *solution) {
    for(int i = 0; i < 32; i++) {
        if(solution[i] != 0xff) {
            int degree = solution[i] % 3, face = solution[i] / 3;
            switch(face) {
                case 0: putchar('U'); break;
                case 1: putchar('D'); break;
                case 2: putchar('L'); break;
                case 3: putchar('R'); break;
                case 4: putchar('B'); break;
                case 5: putchar('F'); break;
            }
            switch(degree) {
                case 1: putchar('\''); break;
                case 2: putchar('2');
            }
            putchar(' ');
        }
    }
}

int main() {

    printf("initializing coordinate multiplication tables...\n");
    init_mult_tables();

    printf("initializing pruning tables...\n");
    init_pruning_table();

    Cube cube = create_random_cube();
    print_cube(&cube, true);
    
    int solution[32];
    for(int i = 0; i < 32; i++) {
        solution[i] = 0xff;
    }

    for(int depth = 0; depth < 21; depth++) {
        printf("searching depth %d\n", depth);
        if(search(&cube, -1, 0, depth, solution)) {
            print_solution(solution);
            break;
        }
    }

}