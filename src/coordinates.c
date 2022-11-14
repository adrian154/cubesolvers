#include "coordinates.h"
#include <string.h>
#include <stdlib.h>

unsigned int *co_mult_table;
unsigned int *eo_mult_table;
unsigned int *ec_mult_table;

// convert move to integer {0..17}
int move_to_int(int  face, int degree) {
    return face * 3 + degree;
}

/*
 * The orientation of the last corner cubie depends on the orientation of the
 * others, so there are only really 7 degrees of freedom.
 */
int compute_co_coord(Cube *cube) {
    int coord = 0, base = 1;
    for(int i = 0; i < 7; i++) {
        coord += cube->corner_orientations[cube->corners[i]] * base;
        base *= 3;
    }
    return coord;
}

int compute_eo_coord(Cube *cube) {
    int coord = 0;
    for(int i = 0; i < 11; i++) {
        coord |= cube->edge_orientations[cube->edges[i]] << i;
    }
    return coord;
}

int compute_ec_coord(Cube *cube) {

    int edge_pos = 0, corner_pos = 0;

    for(int i = 0; i < 12; i++) {
        if(cube->edges[i] == 0) {
            edge_pos = i;
            break;
        }
    }

    for(int i = 0; i < 8; i++) {
        if(cube->corners[i] == 0) {
            corner_pos = i;
            break;
        }
    }

    return corner_pos * 12 + edge_pos;

}

/*
 * eslice_raw represents the position of the four E-slice edge cubies. It has
 * 12P4 = 11880 possible values, which are reduced to <TODO> equivalence 
 * classes via symmetry reduction.
 * 
 * The process we use to map permutations to integers is known  as a Lehmer
 * code. In a nutshell, we convert each element of the permutation at position
 * `i` to a value {0..i-1} by counting how many elements smaller than the 
 * current one occur to the left of it. We can then interpret these values as
 * digits of a factorial-base number, producing our index.
 */
int compute_eslice_raw_coord(Cube *cube) {
    
    /*
     * Since we're dealing with a partial permutation, our base is nPr instead
     * of n!.
     */
    static int base[] = {990, 90, 9, 1};

    /*
     * We record which elements we've seen so far in a bitmap. This allows us
     * to determine how many elements smaller than the current one have been
     * encountered already by using POPCNT. This is based on the approach
     * suggested by Richard Korf in "Large-Scale Parallel Breadth-First Search",
     * except he used the shifted mask as an index into a lookup table (Which
     * is probably slower on modern hardware). 
     */
    int seen_mask = 0, coord = 0;
    for(int i = 0; i < 4; i++) {
        int cubie = cube->corners[i];
        seen_mask |= 1 << (3 - cubie);
        coord += (cubie - __builtin_popcount(seen_mask >> (4 - cubie))) * base[i];
    }

    return coord;

}

void init_co_mult_table() {

    co_mult_table = malloc(18 * 2187 * sizeof(unsigned int)); // 3^7 = 2187

    for(int coord = 0; coord < 2187; coord++) {

        uint8_t corner_orientation[8];
        int coord_copy = coord, total_co = 0;
        
        /*
         * Convert coordinate back into corner orientation values. We can
         * do this without worrying about cubie permutation because we're
         * operating on a solved cube.
         */
        for(int i = 0; i < 7; i++) {
            int co = coord_copy % 3;
            corner_orientation[i] = co;
            total_co += co;
            coord_copy /= 3;
        }

        corner_orientation[7] = (3 - total_co % 3) % 3;
        
        for(int face = 0; face < 6; face++) {
            for(int degree = 0; degree < 3; degree++) {

                // convert the coordinate into a cube
                Cube cube = create_solved_cube();
                memcpy(cube.corner_orientations, corner_orientation, 8);

                // calculate new coordinate and store in table
                do_move(&cube, face, degree);
                co_mult_table[coord * 18 + move_to_int(face, degree)] = compute_co_coord(&cube);

            }
        }
    }

}

void init_eo_mult_table() {

    eo_mult_table = malloc(18 * 2048 * sizeof(unsigned int)); // 2^11 ^ 2048

    for(int coord = 0; coord < 2048; coord++) {

        uint8_t edge_orientation[12];
        int total_eo = 0;

        for(int i = 0; i < 11; i++) {
            int eo = (coord >> i) & 1;
            edge_orientation[i] = eo;
            total_eo += eo;
        }

        edge_orientation[11] = total_eo % 1;

        for(int face = 0; face < 6; face++) {
            for(int degree = 0; degree < 3; degree++) {
                Cube cube = create_solved_cube();
                memcpy(cube.edge_orientations, edge_orientation, 12);
                do_move(&cube, face, degree);
                eo_mult_table[coord * 18 + move_to_int(face, degree)] = compute_eo_coord(&cube);
            }
        }
    }

}

void init_ec_mult_table() {

    for(int coord = 0; coord < 96; coord++) {

        uint8_t 

    }

}

/*
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

    uint8_t corners[] = {0, 1, 2, 3, 4, 5, 6, 7};
    permute_corners(corners, 8);

}
*/

void init_mult_tables() {
    init_co_mult_table();
    init_eo_mult_table();
}

int mult_co(int co, int move) {
    return co_mult_table[co * 18 + move];
}

int mult_eo(int eo, int move) {
    return eo_mult_table[eo * 18 + move];
}
