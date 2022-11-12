// Explicit 3x3x3 Rubik's cube model
#ifndef __CUBE_H
#define __CUBE_H

#include <stdbool.h>
#include <stdint.h>

// Corner positions 
#define ULB 0
#define ULF 1
#define URB 2
#define URF 3
#define DLB 4
#define DLF 5
#define DRB 6
#define DRF 7

// Edge positions
#define UL 0
#define UR 1
#define UB 2
#define UF 3
#define DL 4
#define DR 5
#define DB 6
#define DF 7
#define FL 8
#define FR 9
#define BL 10
#define BR 11

// Cube faces
#define FACE_U 0
#define FACE_D 1
#define FACE_L 2
#define FACE_R 3
#define FACE_B 4
#define FACE_F 5

// Turn degrees
#define TURN_CW   0
#define TURN_CCW  1
#define TURN_FLIP 2

/*
 * The representation we choose for our Rubik's cube will have a large impact on
 * how many positions we are able to search per second. Since we need to index
 * into a pattern database based on the permutation and orientation of certain
 * pieces of the cube, we will represent the cube using the position and orien-
 * tation of each distinct element.
 * 
 * CORNER REPRESENTATION
 * 
 * Each corner position on the cube is given an arbitrary index, from 0 to 7.
 * Corner cubies are referred to by their position in the solved state. For
 * example, when holding the cube with white on top and green in front, the
 * white- orange-green cubie is in the upper left front position. Thus, we call
 * this cubie the ULF cubie. When solved, cubie 0 is in position 0, cubie 1 is
 * in position 1, etc. 
 * 
 * In addition to its position, each corner can also be in one of three orien-
 * tations. Every corner is always in the top or bottom slice, so we choose to
 * use this slice as the up direction in our frame of reference. An orientation
 * of 1 represents one twist clockwise, and an orientation of 2 represents two
 * twists clockwise. The benefit of this system is that the total corner
 * orientation will always be divisible by three.
 * 
 * EDGE REPRESENTATION
 * 
 * Edges are similar to corners, except there are now 12 possible positions.
 * Orientation is defined in the same way as corners, but edges can only be in
 * two different orientations. 
 * 
 * Unlike corner orientation, there is an established convention for edge
 * orientation in the speedsolving community. We have taken care to assign the
 * edge constants in a way such that only F/B quarter turns affect EO,
 * matching this convention.
 * 
 */
typedef struct {
    uint8_t corners[8];
    uint8_t corner_orientations[8];
    uint8_t edges[12];
    uint8_t edge_orientations[12];
} Cube;

bool is_solved(Cube *cube);
Cube create_solved_cube();
Cube create_random_cube();
bool validate_cube(Cube *cube);
void print_cube_raw(Cube *cube);
void get_cube_colors(Cube *cube, char *colors);
void print_cube(Cube *cube, bool terminal);
void do_move(Cube *cube, int face, int degree);
void do_moves(Cube *cube, const char *moves);

#endif