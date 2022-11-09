#include "coordinates.h"
#include "3x3.h"

int main() {

    printf("initializing coordinate multiplication tables...");
    init_mult_tables();

    srand(time(NULL));

    Cube cube = create_random_cube();
    printf("initial cube:\n");
    print_cube(&cube, true);

    int co = compute_co_coord(&cube),
        cp = compute_cp_coord(&cube);
    printf("co = %d, cp = %d\n", co, cp);

    do_move(&cube, FACE_F, TURN_CW);

    printf("after F:\n");
    print_cube(&cube, true);
    printf("actual co = %d, actual cp = %d\n", compute_co_coord(&cube), compute_cp_coord(&cube));

    int move = move_to_int(FACE_F, TURN_CW);
    printf("computed co = %d, computed cp = %d\n", mult_co(co, move), mult_cp(cp, move));

}