#ifndef __PRUNE_TABLE_H
#define __PRUNE_TABLE_H

/*
 * Our algorithm of choice for searching the Rubik's cube game tree is iter-
 * ative deepening A*. In a nutshell, IDA* conducts depth first searches of
 * progressively increasing depth until a solution is found; because each DFS is
 * depth-limited, if we have a heuristic that gives us a lower bound on the
 * number of moves necessary to solve any state, we can prune branches where the
 * current depth plus the value of the heuristic exceeds the depth limit.
 * 
 * We construct an admissible heuristic (one that never overestimates the number
 * of remaining moves) using the following strategy:
 * 
 *   - Identify a target subgroup of the cube group of which the solved state
 *     is a member.
 *   - Observe that within the cosets of this subgroup, every cube state within
 *     the coset is the same "distance" (number of moves) away from the 
 *     subgroup.
 *   - Thus, we only need to record one value per coset. Cube states are mapped
 *     to cosets via coordinates (see coordinates.h).
 * 
 * We build our pruning tables using the strategy suggested by Kociemba: we
 * use the pruning table to conduct a BFS of cosets, exploring until the table
 * is populated. This unfortunately means that we are forced to search at a
 * branching factor of 18 (since without information on the last turned face
 * we cannot eliminate certin branches), but that's fine. 
 */

void init_pruning_tables();

#endif