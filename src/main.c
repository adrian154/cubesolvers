#include "search.h"

int main() {

    printf("initializing coordinate multiplication tables...\n");
    init_mult_tables();

    printf("initializing pruning tables...\n");
    init_pruning_tables();

}