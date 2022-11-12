#ifndef __COORDINATES_H
#define __COORDINATES_H

#include <stdint.h>
#include "cube.h"

/*
 * The Rubik's Cube group, usually just referred to as the "cube group" or G,
 * consists of the identity permutation and six face quarter turns
 * <L,R,F,B,U,D>. The group operation is the composition of permutations (i.e.
 * applying a sequence of moves to a cube state).
 * 
 * Our goal is to create a heuristic that gives us a lower bound on the number
 * of moves necessary to solve any cube state. We start by picking an 
 * arbitrary subgroup of the cube group which the solved state is a member of.
 * We observe that within cosets of this subgroup, every cube state within
 * the coset is the same distance (number of moves) away from the subgroup.
 * Thus, we only need to record one value per coset. We map cubes to cosets
 * using coordinates.
 *
 * For example, consider the subgroup of cubes where the corners are solved.
 * Within cosets of this subgroup, every member has the same corner orien-
 * tation and permutation. Thus, we can map any cube state to a coset of this
 * subgroup by calculating a unique integer that represents the corner orien-
 * tation and permutation. 
 * 
 * This mapping produces 8!*3^7 cosets. The size of each coset is equal to
 * |G|/(8!*3^7) = 490,497,638,400. This value makes sense; there are 
 * 12!*2^11 = 490,497,638,400 x 2 = 980,995,276,800 possible combinations of
 * edge orientation and permutation, but half of these have mismatched parity.
 *
 * We choose to work with coordinates instead of the cubie-based cube model in
 * our solver. Instead of applying a move to a Cube and recalculating the 
 * coordinate values, we use lookup tables to simulate the application of a 
 * move. This greatly speeds up pruning table generation.
 * 
 * SYMMETRY
 * 
 * The size of the pruning table is the limiting factor for how quickly we can
 * search the problem space. Exploiting the cube's symmetrical qualities allow
 * us to store a larger pruning table in a smaller amount of space. Intuitively,
 * if we mirror a cube, it will still require the same amount of moves to solve.
 * Our goal is to develop a coordinate that recognizes the  equivalence of these
 * states and maps them to the same value.
 */

void init_mult_tables();
int compute_co_coord(Cube *cube);
int compute_cp_coord(Cube *cube);
int compute_eop1_coord(Cube *cube, int edge);
int mult_co(int co, int move);
int mult_cp(int cp, int move);
int mult_eop1(int eop1, int move);
int move_to_int(int face, int degree);

#endif