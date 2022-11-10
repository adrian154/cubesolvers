#include "search.h"
#include "coordinates.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define TABLE_SIZE 88179840

uint8_t *table;

void init_pruning_tables() {

    table = malloc(TABLE_SIZE);
    for(int i = 0; i < TABLE_SIZE; i++) {
        table[i] = 0xff;
    }

    table[0] = 0;

    int depth = 0;
    int nodes_explored = 1;

    /*
     * Repeatedly scan through the table, searching for nodes that were just
     * explored. When one of these nodes is found, perform the 18 possible moves
     * on it and record in the table.
     */
    while(nodes_explored > 0) {

        nodes_explored = 0;
        for(int i = 0; i < TABLE_SIZE; i++) {
            
            if(table[i] == depth) {

                int cp = i / 2187, co = i % 2187;
                for(int move = 0; move < 18; move++) {

                    int next_coord = mult_cp(cp, move) * 2187 + mult_co(co, move);
                    if(table[next_coord] > depth + 1) {
                        table[next_coord] = depth + 1;
                    }

                    nodes_explored++;

                }

            }

        }

        printf("%d nodes explored at depth %d\n", nodes_explored, depth);
        depth++;

    }

    FILE *fp = fopen("corners.prune", "wb");
    fwrite(table, 1, TABLE_SIZE, fp);
    fclose(fp);    

}