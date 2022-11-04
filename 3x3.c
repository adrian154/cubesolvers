// See 3x3.h for details on cube representation.
#include "3x3.h"
#include <stdio.h>

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

Cube create_cube() {
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

// Convert face color to ANSI escape code
const char *get_escape(char color) {
    switch(color) {
        case 'R': return "\x1b[48;2;255;0;0m";
        case 'O': return "\x1b[48;2;255;128;0m";
        case 'Y': return "\x1b[48;2;255;255;0m";
        case 'G': return "\x1b[48;2;0;255;0m";;
        case 'B': return "\x1b[48;2;0;0;255m";
        case 'W': return "\x1b[48;2;255;255;255m";
        default: return "\x1b[0m";
    }
}

// See get_cube_colors() for info on what this function does.
int get_diagonal(int corner_pos) {
    return corner_pos == ULF ||
           corner_pos == URB ||
           corner_pos == DLB ||
           corner_pos == DRF;
}

// Color the facelets for a corner cubie
void color_corner(char colors[], int corner_pos, char color1, char color2, char color3) {
    switch(corner_pos) {
        case ULB: colors[12] = color1; colors[11] = color2; colors[6]  = color3; break;
        case ULF: colors[36] = color1; colors[35] = color2; colors[45] = color3; break;
        case URB: colors[14] = color1; colors[15] = color2; colors[8]  = color3; break;
        case URF: colors[38] = color1; colors[39] = color2; colors[47] = color3; break;
        case DLB: colors[20] = color1; colors[9]  = color2; colors[0]  = color3; break;
        case DLF: colors[44] = color1; colors[33] = color2; colors[51] = color3; break;
        case DRB: colors[18] = color1; colors[17] = color2; colors[2]  = color3; break;
        case DRF: colors[42] = color1; colors[41] = color2; colors[53] = color3; break;
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
        char color1 = cubie & 4 ? 'Y' : 'W',
             color2 = cubie & 2 ? 'R' : 'O',
             color3 = cubie & 1 ? 'G' : 'B';

        /*
         * The cubie's orientation tells us the color of one of the stickers,
         * but the order of the other two depends on whether the cubie's current
         * position is adjacent or diagonal from its home position.
         */
        int orientation = cube->corner_orientations[cubie];
        if(get_diagonal(cubie) == get_diagonal(corner_pos)) {
            if(orientation == 0)
                color_corner(colors, corner_pos, color1, color2, color3);
            else if(orientation == 1)
                color_corner(colors, corner_pos, color3, color1, color2);
            else
                color_corner(colors, corner_pos, color2, color3, color1);
        } else {
            if(orientation == 0)
                color_corner(colors, corner_pos, color1, color3, color2);
            else if(orientation == 1)
                color_corner(colors, corner_pos, color2, color1, color3);
            else
                color_corner(colors, corner_pos, color3, color2, color1);
        }

    }

    for(int edge_pos = 0; edge_pos < 12; edge_pos++) {
        
        int edge = cube->edges[edge_pos];

        // I couldn't come up with a more clever way to encode the edges, so...
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

/*
 * If a corner is facing the same direction as the axis of a quarter turn, its
 * orientation isn't affected. Otherwise, it swaps between the two other 
 * possible values. 
 * 
 * Example: if a cubie in the U face has orientation 0 (facing up), it will
 * remain in that orientation. Thus, we say the axis of a U-turn is 0.
 */
void orient_corner(Cube *cube, int corner_pos, int axis) {
    
    int cubie = cube->corners[corner_pos];
    int orientation = cube->corner_orientations[cubie];
    
    static int orientations[] = {
        0, 2, 1,
        2, 1, 0,
        1, 0, 2
    };

    cube->corner_orientations[cubie] = orientations[axis * 3 + orientation];

}

// flip orientation of an edge
void orient_edge(Cube *cube, int edge) {
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

        orient_corner(cube, corner0, axis);
        orient_corner(cube, corner1, axis);
        orient_corner(cube, corner2, axis);
        orient_corner(cube, corner3, axis);

        // only F/B quarter turns affect edge orientation
        if(axis == 2) {
            orient_edge(cube, cube->edges[edge0]);
            orient_edge(cube, cube->edges[edge1]);
            orient_edge(cube, cube->edges[edge2]);
            orient_edge(cube, cube->edges[edge3]);
        }

    }

}

void do_move(Cube *cube, int face, int degree) {
    switch(face) {
        case FACE_U: apply_turn(cube, 0, degree, ULF, ULB, URB, URF, UB, UR, UF, UL); break;
        case FACE_D: apply_turn(cube, 0, degree, DRF, DRB, DLB, DLF, DF, DR, DB, DL); break;
        case FACE_L: apply_turn(cube, 1, degree, ULB, ULF, DLF, DLB, UL, FL, DL, BL); break;
        case FACE_R: apply_turn(cube, 1, degree, URF, URB, DRB, DRF, UR, BR, DR, FR); break;
        case FACE_B: apply_turn(cube, 2, degree, URB, ULB, DLB, DRB, UB, BL, DB, BR); break;
        case FACE_F: apply_turn(cube, 2, degree, URF, DRF, DLF, ULF, UF, FR, DF, FL); break;
    }
}
