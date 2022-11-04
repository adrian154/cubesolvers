#ifndef __PRUNING_H
#define __PRUNING_H

#include "3x3.h"

/*
 * Our algorithm of choice for searching the Rubik's cube game tree is iter-
 * ative deepening A*. In a nutshell, IDA* conducts depth first searches of
 * progressively increasing depth; because each DFS is depth-limited, if we have
 * a heuristic that gives us a lower bound on the number of moves necessary to
 * solve any state, we can prune branches where the current depth plus the value
 * of the heuristic exceeds the depth limit.
 * 
 * We construct an admissible heuristic (never overestimate number of remaining
 * moves) through the use of pattern databases/pruning tables. It's not possible
 * for us to index the number of moves required to solve every one of the 43e19
 * reachable cube states, but we can create pattern databases cataloguing the
 * number of moves to solve a *subproblem* of solving the cube. For example,
 * we can determine the number of moves necessary to solve every possible 
 * combination of corner pieces; this is an admissible heuristic because any
 * solution that solves the cube will also solve the corners.
 * 
 * PATTERN DATABASE INDEXING
 * 
 * The scheme we use to map cube states to pattern database indexes should
 * ideally map perfectly to some range of integers, while also being fast to
 * compute. For edge/corner orientations, this is easy; each orientation is
 * independent, so they can be treated as numbers of base 2 and 3, respectively.
 * However, this is not the case for permutations. In this case, a Lehmer code
 * can be used to map each possible permutation to an integer {0..n!-1}. We can
 * calculate this value in O(n) space *and* time using the approach outlined by
 * Korf & Schultze in "Large-Scale Parallel Breadth-First Search", although for
 * practical purposes since n is usually small, the improved time complexity
 * (compared with the O(n^2) naive approach) is the principal advantage.
 * 
 * Kociemba refers to these indexes as coordinates in his writings on the cube,
 * so we'll call them coordinates too for the sake of clarity.
 */
int get_corner_coordinate(Cube *cube);

#endif