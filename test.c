#include "3x3.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

    Cube cube = create_cube();
    for(int i = 1; i < argc; i++) {

        char *move = argv[i];
        int face;
        switch(move[0]) {
            case 'F': face = FACE_F; break;
            case 'B': face = FACE_B; break;
            case 'L': face = FACE_L; break;
            case 'R': face = FACE_R; break;
            case 'U': face = FACE_U; break;
            case 'D': face = FACE_D; break;
            default: printf("'%c' isn't a valid face\n", move[0]); return 1;
        }

        int degree;
        switch(move[1]) {
            case 'i': degree = TURN_CCW; break;
            case '2': degree = TURN_FLIP; break;
            case '\0': degree = TURN_CW; break;
            default: printf("unexpected '%c'\n", move[1]); return 1;
        }

        printf("after %s:\n", move);
        do_move(&cube, face, degree);
        print_cube(&cube, true);

    }

}