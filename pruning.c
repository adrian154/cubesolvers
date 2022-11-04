#include "pruning.h"

int get_corner_coordinate(Cube *cube) {
    
    static int factorial[] = {5040, 720, 120, 24, 6, 2, 1};

    /* 
     * The orientation of the 8th cubie depends on the orientation of the other
     * seven, so we don't need to encode it.
     */
    int orientation_coord = 0, base = 1;
    for(int i = 0; i < 7; i++) {
        orientation_coord += base * cube->corner_orientations[i];
        base *= 3;
    }

    /*
     * Use Korf's bitstring approach to calculate Lehmer code in linear time.
     * We can further speed it up by using popcount instead of indexing into a
     * table as he originally suggested.
     */
    int seen_mask = 0, corner_coord = 0;
    for(int i = 0; i < 7; i++) {
        
        int cubie = cube->corners[i];
        
        // Mark that we saw the cubie by setting a bit in the mask
        seen_mask |= 1 << (7 - cubie);

        /*
         * In a Lehmer code, we look at each element and subtract the number of
         * elements to the left of it in the permutation whose value is lower
         * than the element itself.
         * 
         * We can treat the resulting values as digits of a factoradic number
         * and convert it to a regular integer, producing a unique mapping for
         * each valid permutation.
         */
        // TODO: Make this code portable (fallback for __builtin_popcount)
        corner_coord += (cubie - __builtin_popcount(seen_mask >> (8 - cubie))) * factorial[i];

    }

    // orientation_coord is in {0..2186}
    return corner_coord * 2187 + orientation_coord;

}