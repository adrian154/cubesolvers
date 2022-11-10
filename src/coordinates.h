#ifndef __COORDINATES_H
#define __COORDINATES_H

#include <stdint.h>
#include "cube.h"

/*
 * The Rubik's Cube group, usually just referred to as the cube group or G,
 * consists of the identity permutation and six face quarter turns
 * <L,R,F,B,U,D>. The group operation is the composition of permutations (i.e.
 * applying a sequence of moves to a cube state).
 * 
 * When creating a heuristic, what we're doing is building a pattern database
 * that tells us how many moves are necessary to bring any cube state to a 
 * certain subgroup. (The solved state has to be a member of this subgroup.)    
 * For example, suppose we decided to target the subgroup of all cubes where
 * the corners have been solved. Intuitively, all cube states with the same
 * corner permutation and corner orientation will require the same number of
 * moves to solve the corners, so we can organize the members of the cube group
 * into 8!*3^7 cosets. Every member of the coset is the same distance from the
 * target subgroup, so we only need to record 8!*3^7 values instead of |G|.
 *
 * We choose to work with coordinates instead of the cubie-based cube model in
 * our solver. Instead of recalculating coordinates after moves are applied, we
 * use fixed lookup tables for coordinate multiplication.
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