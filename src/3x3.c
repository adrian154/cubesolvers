// See 3x3.h for details on cube representation.
#include "3x3.h"
#include <stdio.h>
#include <stdlib.h>

// Check if a cube is solved.
bool is_solved(Cube *cube) {

    // check corner permutation and orientation
    for(int i = 0; i < 8; i++) {
        if(cube->corners[i] != i || cube->corner_orientations[i] != 0) {
            return false;
        }
    }

    // check edge permutation and orientation
    for(int i = 0; i < 12; i++) {
        if(cube->edges[i] != i || cube->edge_orientations[i] != 0) {
            return false;
        }
    }

    return true;

}

Cube create_solved_cube() {
    Cube cube;
    for(int i = 0; i < 8; i++) {
        cube.corners[i] = i;
        cube.corner_orientations[i] = 0;
    }
    for(int i = 0; i < 12; i++) {
        cube.edges[i] = i;
        cube.edge_orientations[i] =0;
    }
    return cube;
}

// Fisher-Yates shuffle
void shuffle(uint8_t *list, int size) {

    for(int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = list[i];
        list[i] = list[j];
        list[j] = tmp;
    }

}

// determine parity of a permutation by counting inversions
int compute_parity(uint8_t *list, int size) {
    int inversions = 0;
    for(int i = 0; i < size; i++) {
        for(int j = i + 1; j < size; j++) {
            if(list[i] > list[j]) inversions++;
        }
    }
    return inversions % 2;
}

bool validate_cube(Cube *cube) {

    int seen_mask = 0;
    for(int i = 0; i < 8; i++) {
        int cubie = cube->corners[i];
        if(cubie > 7) {
            printf("illegal corner %d in position %d\n", cubie, i);
            return false;
        }
        if(seen_mask & (1 << cubie)) {
            printf("corner %d occurs more than once (last seen at %d)\n", cubie, i);
            return false;
        }
        seen_mask |= 1 << cubie;
    }

    seen_mask = 0;
    for(int i = 0; i < 12; i++) {
        int cubie = cube->edges[i];
        if(cubie > 11) {
            printf("illegal edge %d in position %d\n", cubie, i);
            return false;
        }
        if(seen_mask & (1 << cubie)) {
            printf("edge %d occurs more than once (last seen at %d)\n", cubie, i);
            return false;
        }
        seen_mask |= 1 << cubie;
    }

    int total_eo = 0;
    for(int i = 0; i < 12; i++) {
        if(cube->edge_orientations[i] > 1) {
            printf("illegal orientation %d for edge cubie %d\n", cube->edge_orientations[i], i);
            return false;
        }
        total_eo += cube->edge_orientations[i];
    }

    if(total_eo % 2 != 0) {
        printf("corner orientation not divisible by 3 (%d)\n", total_eo);
        return false;
    }

    int total_co = 0;
    for(int i = 0; i < 8; i++) {
        if(cube->corner_orientations[i] > 2) {
            printf("illegal orientation %d for corner cubie %d\n", cube->corner_orientations[i], i);
            return false;
        }
        total_co += cube->corner_orientations[i];
    }

    if(total_co % 3 != 0) {
        printf("corner orientation not divisible by 3 (%d)\n", total_co);
        return false;
    }

    return true;

}

void print_cube_raw(Cube *cube) {

    printf("corners: ");
    for(int i = 0; i < 8; i++) {
        printf("%d=%d/%d ", i, cube->corners[i], cube->corner_orientations[cube->corners[i]]);
    }

    printf("\nedges: ");
    for(int i = 0; i < 12; i++) {
        printf("%d=%d/%d ", i, cube->edges[i], cube->edge_orientations[cube->edges[i]]);
    }
    putchar('\n');

}

// create solvable cube
Cube create_random_cube() {
    
    Cube cube;

    for(int i = 0; i < 8; i++) {
        cube.corners[i] = i;
    }

    for(int i = 0; i < 12; i++) {
        cube.edges[i] = i;
    }

    shuffle(cube.corners, 8);
    shuffle(cube.edges, 12);

    // CONSTRAINT: corner permutation parity must match edge permutation parity
    if(compute_parity(cube.corners, 8) != compute_parity(cube.edges, 12)) {

        // change parity of edges by doing a swap
        int tmp = cube.edges[0];
        cube.edges[0] = cube.edges[1];
        cube.edges[1] = tmp;

    }

    // CONSTRAINT: total edge orientation must be even
    int total_eo = 0;
    for(int i = 0; i < 11; i++) {
        int eo = rand() & 1;
        total_eo += eo;
        cube.edge_orientations[i] = eo;
    }

    cube.edge_orientations[11] = total_eo % 2;

    // CONSTRAINT: total corner orientations must be divisible by three
    int total_co = 0;
    for(int i = 0; i < 7; i++) {
        int co = rand() % 3;
        total_co += co;
        cube.corner_orientations[i] = co;    
    }

    cube.corner_orientations[7] = (3 - total_co % 3) % 3;

    return cube;

}

// Convert face color to ANSI escape code
const char *get_escape(char color) {
    switch(color) {
        case 'R': return "\x1b[48;2;255;0;0m";
        case 'O': return "\x1b[48;2;255;128;0m";
        case 'Y': return "\x1b[48;2;252;219;3m";
        case 'G': return "\x1b[48;2;0;255;0m";;
        case 'B': return "\x1b[48;2;0;0;255m";
        case 'W': return "\x1b[48;2;255;255;255m";
        default: return "\x1b[0m";
    }
}

// Color the facelets for a corner cubie
void color_corner(char colors[], int corner_pos, char color1, char color2, char color3) {
    switch(corner_pos) {
        case ULB: colors[12] = color1; colors[11] = color2; colors[6]  = color3; break;
        case ULF: colors[36] = color1; colors[45] = color2; colors[35] = color3; break;
        case URB: colors[14] = color1; colors[8]  = color2; colors[15] = color3; break;
        case URF: colors[38] = color1; colors[39] = color2; colors[47] = color3; break;
        case DLB: colors[20] = color1; colors[0]  = color2; colors[9]  = color3; break;
        case DLF: colors[44] = color1; colors[33] = color2; colors[51] = color3; break;
        case DRB: colors[18] = color1; colors[17] = color2; colors[2]  = color3; break;
        case DRF: colors[42] = color1; colors[53] = color2; colors[41] = color3; break;
    }
}

// Color the facelets for an edge cubie
void color_edge(char colors[], int edge_pos, char color1, char color2) {
    switch(edge_pos) {
        case UL: colors[24] = color1; colors[23] = color2; break;
        case UR: colors[26] = color1; colors[27] = color2; break;
        case UB: colors[13] = color1; colors[7]  = color2; break;
        case UF: colors[37] = color1; colors[46] = color2; break;
        case DL: colors[32] = color1; colors[21] = color2; break;
        case DR: colors[30] = color1; colors[29] = color2; break;
        case DB: colors[19] = color1; colors[1]  = color2; break;
        case DF: colors[43] = color1; colors[52] = color2; break;
        case FL: colors[48] = color1; colors[34] = color2; break;
        case FR: colors[50] = color1; colors[40] = color2; break;
        case BL: colors[3] = color1; colors[10]  = color2; break;
        case BR: colors[5] = color1; colors[16]  = color2; break;
    }
}

void print_row(char colors[], int start_idx, int end_idx, bool terminal) {
    for(int i = start_idx; i < end_idx; i++) {
        if(terminal) {
            fputs(get_escape(colors[i]), stdout);
            putchar(' ');
        } else {
            putchar(colors[i]);
        }        
    }
    if(terminal) {
        fputs("\x1b[0m", stdout);
    }
}

/*
 * Convert cube state into an array of face colors. Values are indexed into 
 * `colors` according to the following pattern:
 * 
 *              0   1   2
 *              3   4   5
 *              6   7   8
 * 9    10  11  12  13  14  15  16  17  18  19  20
 * 21   22  23  24  25  26  27  28  29  30  31  32
 * 33   34  35  36  37  38  39  40  41  42  43  44
 *              45  46  47
 *              48  49  50
 *              51  52  53
 * 
 * orientation: 
 *    B
 *  L U R D
 *    F
 * 
 */
void get_cube_colors(Cube *cube, char *colors) {

    // The centers on each face never move, so we can set those easily.
    colors[4]  = 'B';
    colors[25] = 'W';
    colors[49] = 'G';
    colors[22] = 'O';
    colors[28] = 'R';
    colors[31] = 'Y';

    for(int corner_pos = 0; corner_pos < 8; corner_pos++) {

        int cubie = cube->corners[corner_pos];

        /*
         * Figure out the colors of the cubie.  We use the WCA scrambling start
         * position as the orientation for our cube, so white is on top and 
         * green is in front.
         */
        char color1, color2, color3;
        switch(cubie) {
            case ULB: color1 = 'W'; color2 = 'O'; color3 = 'B'; break;
            case ULF: color1 = 'W'; color2 = 'G'; color3 = 'O'; break;
            case URB: color1 = 'W'; color2 = 'B'; color3 = 'R'; break;
            case URF: color1 = 'W'; color2 = 'R'; color3 = 'G'; break;
            case DLB: color1 = 'Y'; color2 = 'B'; color3 = 'O'; break;
            case DLF: color1 = 'Y'; color2 = 'O'; color3 = 'G'; break;
            case DRB: color1 = 'Y'; color2 = 'R'; color3 = 'B'; break;
            case DRF: color1 = 'Y'; color2 = 'G'; color3 = 'R'; break;
        }

        // Shift colors based on the piece's orientation
        int orientation = cube->corner_orientations[cubie];
        if(orientation == 0)
            color_corner(colors, corner_pos, color1, color2, color3);
        else if(orientation == 1)
            color_corner(colors, corner_pos, color3, color1, color2);
        else
            color_corner(colors, corner_pos, color2, color3, color1);

    }

    for(int edge_pos = 0; edge_pos < 12; edge_pos++) {
        
        int edge = cube->edges[edge_pos];

        char color1 = '?', color2 = '?';
        switch(edge) {
            case UL: color1 = 'W'; color2 = 'O'; break;
            case UR: color1 = 'W'; color2 = 'R'; break;
            case UB: color1 = 'W'; color2 = 'B'; break;
            case UF: color1 = 'W'; color2 = 'G'; break;
            case DL: color1 = 'Y'; color2 = 'O'; break;
            case DR: color1 = 'Y'; color2 = 'R'; break;
            case DB: color1 = 'Y'; color2 = 'B'; break;
            case DF: color1 = 'Y'; color2 = 'G'; break;
            case FL: color1 = 'G'; color2 = 'O'; break;
            case FR: color1 = 'G'; color2 = 'R'; break;
            case BL: color1 = 'B'; color2 = 'O'; break;
            case BR: color1 = 'B'; color2 = 'R'; break;
        }

        if(cube->edge_orientations[edge] == 0)
            color_edge(colors, edge_pos, color1, color2);
        else    
            color_edge(colors, edge_pos, color2, color1);

    } 

}

/*
 * Display a cube as a net. `terminal` determines whether ANSI escape codes
 * should be used.
 */
void print_cube(Cube *cube, bool terminal) {

    char colors[54];
    get_cube_colors(cube, colors);

    fputs("    ", stdout); print_row(colors, 0, 3, terminal); putchar('\n');
    fputs("    ", stdout); print_row(colors, 3, 6, terminal); putchar('\n');
    fputs("    ", stdout); print_row(colors, 6, 9, terminal); putchar('\n');
    putchar('\n');

    print_row(colors, 9, 12, terminal); putchar(' '); print_row(colors, 12, 15, terminal);  putchar(' '); print_row(colors, 15, 18, terminal); putchar(' '); print_row(colors, 18, 21, terminal); putchar('\n');
    print_row(colors, 21, 24, terminal); putchar(' '); print_row(colors, 24, 27, terminal);  putchar(' '); print_row(colors, 27, 30, terminal); putchar(' '); print_row(colors, 30, 33, terminal); putchar('\n');
    print_row(colors, 33, 36, terminal); putchar(' '); print_row(colors, 36, 39, terminal);  putchar(' '); print_row(colors, 39, 42, terminal); putchar(' '); print_row(colors, 42, 45, terminal); putchar('\n');
    putchar('\n');

    fputs("    ", stdout); print_row(colors, 45, 48, terminal); putchar('\n');
    fputs("    ", stdout); print_row(colors, 48, 51, terminal); putchar('\n');
    fputs("    ", stdout); print_row(colors, 51, 54, terminal); putchar('\n');
    putchar('\n');

}

void update_corner_orientation(Cube *cube, int corner_pos, int amount) {
    int cubie = cube->corners[corner_pos];
    cube->corner_orientations[cubie] = (cube->corner_orientations[cubie] + amount) % 3;
}

// flip orientation of an edge
void flip_edge(Cube *cube, int edge) {
    cube->edge_orientations[edge] = !cube->edge_orientations[edge];
}

// `corner0..3` and `edge0..3` are lists of corner/edge positions, clockwise
void apply_turn(Cube *cube, int axis, int degree, int corner0, int corner1, int corner2, int corner3, int edge0, int edge1, int edge2, int edge3) {

    if(degree == TURN_FLIP) {

        // swap corners
        int temp = cube->corners[corner0];
        cube->corners[corner0] = cube->corners[corner2];
        cube->corners[corner2] = temp;

        temp = cube->corners[corner1];
        cube->corners[corner1] = cube->corners[corner3];
        cube->corners[corner3] = temp;
        
        // swap edges
        temp = cube->edges[edge0];
        cube->edges[edge0] = cube->edges[edge2];
        cube->edges[edge2] = temp;

        temp = cube->edges[edge1];
        cube->edges[edge1] = cube->edges[edge3];
        cube->edges[edge3] = temp;

    } else {

        // rotate pieces
        int temp;
        if(degree == TURN_CW) {
            temp = cube->corners[corner3];
            cube->corners[corner3] = cube->corners[corner2];
            cube->corners[corner2] = cube->corners[corner1];
            cube->corners[corner1] = cube->corners[corner0];
            cube->corners[corner0] = temp;
            temp = cube->edges[edge3];
            cube->edges[edge3] = cube->edges[edge2];
            cube->edges[edge2] = cube->edges[edge1];
            cube->edges[edge1] = cube->edges[edge0];
            cube->edges[edge0] = temp;
        } else if(degree == TURN_CCW) {
            temp = cube->corners[corner0];
            cube->corners[corner0] = cube->corners[corner1];
            cube->corners[corner1] = cube->corners[corner2];
            cube->corners[corner2] = cube->corners[corner3];
            cube->corners[corner3] = temp;
            temp = cube->edges[edge0];
            cube->edges[edge0] = cube->edges[edge1];
            cube->edges[edge1] = cube->edges[edge2];
            cube->edges[edge2] = cube->edges[edge3];
            cube->edges[edge3] = temp;
        }

        if(axis != 0) {
            update_corner_orientation(cube, corner0, 2);
            update_corner_orientation(cube, corner1, 1);
            update_corner_orientation(cube, corner2, 2);
            update_corner_orientation(cube, corner3, 1);
        }

        // only F/B quarter turns affect edge orientation
        if(axis == 2) {
            flip_edge(cube, cube->edges[edge0]);
            flip_edge(cube, cube->edges[edge1]);
            flip_edge(cube, cube->edges[edge2]);
            flip_edge(cube, cube->edges[edge3]);
        }

    }

}

void do_move(Cube *cube, int face, int degree) {
    switch(face) {
        case FACE_U: apply_turn(cube, 0, degree, ULB, URB, URF, ULF, UB, UR, UF, UL); break;
        case FACE_D: apply_turn(cube, 0, degree, DRF, DRB, DLB, DLF, DF, DR, DB, DL); break;
        case FACE_L: apply_turn(cube, 1, degree, ULB, ULF, DLF, DLB, UL, FL, DL, BL); break;
        case FACE_R: apply_turn(cube, 1, degree, URF, URB, DRB, DRF, UR, BR, DR, FR); break;
        case FACE_B: apply_turn(cube, 2, degree, URB, ULB, DLB, DRB, UB, BL, DB, BR); break;
        case FACE_F: apply_turn(cube, 2, degree, ULF, URF, DRF, DLF, UF, FR, DF, FL); break;
    }
}

// Apply a space-separated sequence of moves to a cube. 
void do_moves(Cube *cube, const char *moves) {

    int  face = -1;

    while(true) {
        
        char cur = *moves;
        moves++;

        if(face != -1) {
            switch(cur) {
                case '\'': do_move(cube, face, TURN_CCW); break;
                case '2': do_move(cube, face, TURN_FLIP); break;
                case '\0': // <fall through>
                case ' ': do_move(cube, face, TURN_CW); break;
                default:
                    printf("expected turn degree but got '%c'\n", cur);
                    return;
            }
            face = -1;
        } else {
            switch(cur) {
                case 'U': face = FACE_U; break;
                case 'D': face = FACE_D; break;
                case 'L': face = FACE_L; break;
                case 'R': face = FACE_R; break;
                case 'B': face = FACE_B; break;
                case 'F': face = FACE_F; break;
                case ' ': // fall through
                case '\0': break;
                default:
                    printf("expected a cube face but got '%c'\n", cur);
                    return;
            }
        }

        if(cur == '\0') {
            return;
        }

    }

}