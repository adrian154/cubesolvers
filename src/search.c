#include "search.h"
#include "coordinates.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 88179840

uint8_t *table;

void calculate_table_stats() {

    int freq[32];
    for(int i = 0; i < 32; i++) {
        freq[i] = 0;
    }

    int max = 0, sum = 0;
    for(int i = 0; i < TABLE_SIZE; i++) {
        freq[table[i]]++;
        max = table[i] > max ? table[i] : max;
        sum += table[i];
    }

    for(int i = 0; i <= max; i++) {
        printf("depth %d: %d (%.2f%%)\n", i, freq[i], (double)freq[i] * 100 / TABLE_SIZE);
    }

    printf("expected value: %.2f\n", (double)sum / TABLE_SIZE);

}

void build_pruning_table() {

    printf("building pruning table...\n");

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
    if(fp == NULL) {
        perror("failed to open pruning table for writing");
        exit(1);
    }

    if(fwrite(table, 1, TABLE_SIZE, fp) != TABLE_SIZE) {
        perror("failed to write pruning table");
        fclose(fp);
        exit(1);
    }

    fclose(fp);    
    calculate_table_stats();

}

void init_pruning_table() {

    FILE *fp = fopen("corners.prune", "rb");
    if(fp == NULL) {
        perror("couldn't open pruning table");
        build_pruning_table();
    } else {
        fread(table, 1, TABLE_SIZE, fp);
        if(ferror(fp)) {
            perror("failed to read pruning table");
            fclose(fp);
            exit(1);
        }
        fclose(fp);
    }

}