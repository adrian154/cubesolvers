#include "3x3.h"
#include "coordinates.h"

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
void compute_co_coord(Cube *cube) {
    int coord = 0, base = 1;
    for(int i = 0; i < 7; i++) {
        coord += cube->corner_orientations[i] * base;
        base *= 3;
    }
}

/*
 * 
 */
void compute_cp_coord(Cube *cube) {

}

void init_co_mult_table() {

    co_mult_table = malloc(39366 * sizeof(unsigned int)); // 18 * 3^7

    for(int coord = 0; coord < 2187; coord++) {
        for(int face = 0; face < 6; face++) {
            for(int degree = 0; degree < 3; degree++) {

                // convert the coordinate into a cube
                Cube cube = create_solved_cube();
                int coord_copy = coord;
                for(int i = 0; i < 7; i++) {
                    cube.corner_orientations[i] = coord_copy % 3;
                    coord_copy /= 3;
                }

                // calculate new coordinate and store in table
                do_move(&cube, face, degree);
                co_mult_table[coord * 18 + move_to_int(face, degree)] = compute_co_coord(&cube);

            }
        }
    }

}

void init_cp_mult_table() {

    cp_mult_table = malloc(725760 * sizeof(unsigned int)); // 18 * 8!

}

void init_mult_tables() {



}