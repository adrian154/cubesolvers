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
 * PRUNING TABLES
 * 
 * We build our pruning tables using the strategy suggested by Kociemba: we
 * use the pruning table to conduct a BFS of cosets, exploring until the table
 * is populated. This unfortunately means that we are forced to search at a
 * branching factor of 18 (since without information on the last turned face
 * we cannot eliminate redundant branches), but this is not a big problem since
 * the computational cost of building the pruning table is amortized across
 * solves, so as long as it completes in a *reasonable* amount of time per-
 * formance is not important.
 * 
 * See coordinates.h for more information on the mathematics of pruning tables.
 *  
 */

void init_pruning_table();

#endif