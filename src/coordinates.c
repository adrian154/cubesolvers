#include "3x3.h"
#include "coordinates.h"
#include <string.h>
#include <stdlib.h>

unsigned int *co_mult_table;
unsigned int *cp_mult_table;

// convert move to integer {0..17}
int move_to_int(int face, int degree) {
    return face * 3 + degree;
}

/*
 * The orientation of the last cubie depends on the orientation of the others,
 * so there are only really 7 degrees of freedom.
 */
int compute_co_coord(Cube *cube) {
    int coord = 0, base = 1;
    uint8_t corner_orientation[8];
    for(int i = 0; i < 8; i++) {
        corner_orientation[i] = cube->corner_orientations[cube->corners[i]];
    }
    for(int i = 0; i < 7; i++) {
        coord += corner_orientation[i] * base;
        base *= 3;
    }
    return coord;
}

/*
 * Permutations can be mapped to integers {0..n!-1} using Lehmer codes. We
 * convert each element in the permutation to the number of values to the left
 * of it that are smaller, and treat these values as digits of a factorial-base
 * number.
 */
int compute_cp_coord(Cube *cube) {

    static int factorial[] = {5040, 720, 120, 24, 6, 2, 1};

    /*
     * We record which elements we've seen so far in a bitmap. This allows us
     * to determine how many elements smaller than the current one have been
     * encountered already by using POPCNT. This is based on the approach
     * suggested by Richard Korf in "Large-Scale Parallel Breadth-First Search",
     * except he used the shifted mask as an index into a lookup table (Which
     * is probably slower on modern hardware). 
     */
    int seen_mask = 0, coord = 0;
    for(int i = 0; i < 7; i++) {
        int cubie = cube->corners[i];
        seen_mask |= 1 << (7 - cubie);
        coord += (cubie - __builtin_popcount(seen_mask >> (8 - cubie))) * factorial[i];
    }

    return coord;

}

void init_co_mult_table() {

    co_mult_table = malloc(18 * 2187 * sizeof(unsigned int)); // 3^7 = 2187

    for(int coord = 0; coord < 2187; coord++) {
        for(int face = 0; face < 6; face++) {

            uint8_t corner_orientation[7];
            int coord_copy = coord;
            for(int i = 0; i < 7; i++) {
                corner_orientation[i] = coord_copy % 3;
                coord_copy /= 3;
            }

            for(int degree = 0; degree < 3; degree++) {

                // convert the coordinate into a cube
                Cube cube = create_solved_cube();
                memcpy(cube.corner_orientations, corner_orientation, 7);

                // calculate new coordinate and store in table
                do_move(&cube, face, degree);
                co_mult_table[coord * 18 + move_to_int(face, degree)] = compute_co_coord(&cube);

            }
        }
    }

}

/*
 * Recursively generate corner permutations using Heap's algorithm.
 */
void permute_corners(uint8_t *corners, int k) {

    if(k == 1) {

        for(int face = 0; face < 6; face++) {
            for(int degree = 0; degree < 3; degree++) {

                Cube cube = create_solved_cube();
                memcpy(&cube.corners, corners, 8);
                int coord = compute_cp_coord(&cube);

                do_move(&cube, face, degree);
                cp_mult_table[coord * 18 + move_to_int(face, degree)] = compute_cp_coord(&cube);

            }
        }
        
    } else {

        permute_corners(corners, k - 1);
        for(int i = 0; i < k - 1; i++) {
            if(k % 2) {
                int tmp = corners[0];
                corners[0] = corners[k - 1];
                corners[k - 1] = tmp;
            } else {
                int tmp = corners[i];
                corners[i] = corners[k - 1];
                corners[k - 1] = tmp;
            }
            permute_corners(corners, k - 1);
        }

    }

}

void init_cp_mult_table() {

    cp_mult_table = malloc(18 * 40320 * sizeof(unsigned int)); // 8! = 40320

    /*
     * Decoding a Lehmer code is a bit of a pain, so instead we iterate over
     * permutations using 
     */
    uint8_t corners[] = {0, 1, 2, 3, 4, 5, 6, 7};
    permute_corners(corners, 8);

}

void init_mult_tables() {
    init_cp_mult_table();
    init_co_mult_table();
}

int mult_co(int co, int move) {
    return co_mult_table[co * 18 + move];
}

int mult_cp(int cp, int move) {
    return cp_mult_table[cp * 18 + move];
}