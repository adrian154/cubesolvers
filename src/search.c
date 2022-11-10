#include "search.h"
#include "coordinates.h"
#include "3x3.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 2116316160

uint8_t *table;

void calculate_table_stats() {

    int freq[32];
    for(int i = 0; i < 32; i++) {
        freq[i] = 0;
    }

    int max = 0;
    unsigned long long sum = 0;
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

int build_table_index(int eop1, int cp, int co) {
    return eop1 * 88179840 +
           cp * 2187 +
           co;
}

void build_pruning_table() {

    printf("building pruning table...\n");

    for(int i = 0; i < TABLE_SIZE; i++) {
        table[i] = 0xff;
    }

    table[0] = 0;

    int depth = 0;
    unsigned long long nodes_explored = 1;

    /*
     * Repeatedly scan through the table, searching for nodes that were just
     * explored. When one of these nodes is found, perform the 18 possible moves
     * on it and record in the table.
     */
    while(nodes_explored > 0) {

        nodes_explored = 0;
        for(int i = 0; i < TABLE_SIZE; i++) {
            
            if(table[i] == depth) {

                int eop1 = i / 88179840,
                    cp = i / 2187 % 40320,
                    co = i % 2187;

                for(int move = 0; move < 18; move++) {

                    int next_coord = build_table_index(mult_eop1(eop1, move), mult_cp(cp, move), mult_co(co, move));
                    if(table[next_coord] > depth + 1) {
                        table[next_coord] = depth + 1;
                    }

                    nodes_explored++;

                }

            }

        }

        printf("%llu nodes explored at depth %d\n", nodes_explored, depth);
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

    table = malloc(TABLE_SIZE);

    FILE *fp = fopen("corners.prune", "rb");
    if(fp == NULL) {
        perror("couldn't open pruning table");
        build_pruning_table();
    } else {
        printf("loading pruning table...\n");
        fread(table, 1, TABLE_SIZE, fp);
        if(ferror(fp)) {
            perror("failed to read pruning table");
            fclose(fp);
            exit(1);
        }
        fclose(fp);
    }

}

bool search(Cube *cube, int last_turn_face, int depth, int max_depth, int *solution) {

    if(depth == max_depth) {
        return false;
    }

    for(int face = 0; face < 6; face++) {
        
        // don't evaluate moves that would cancel the previous one
        if(last_turn_face == face)
            continue;

        // turns of opposite faces are commutative, we only need to evaluate
        // each combination once
        if((last_turn_face == FACE_D && face == FACE_U) ||
           (last_turn_face == FACE_R && face == FACE_L) ||
           (last_turn_face == FACE_B && face == FACE_F))
            continue;
        
        for(int degree = 0; degree < 3; degree++) {

            Cube next = *cube;
            do_move(&next, face, degree);
            
            // if we've solved the cube, rejoice!
            if(is_solved(&next)) {
                solution[depth] = move_to_int(face, degree);
                return true;
            }

            // try to prune
            int remaining_moves = table[build_table_index(compute_eop1_coord(&next, 0), compute_cp_coord(&next), compute_co_coord(&next))];
            if(depth + remaining_moves >= max_depth) {
                continue;
            }

            // recursively search
            if(search(&next, face, depth + 1, max_depth, solution)) {
                solution[depth] = move_to_int(face, degree);
                return true;
            }

        }

    }

    return false;

}