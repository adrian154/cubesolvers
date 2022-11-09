#include "3x3.h"
#include "pruning.h"
#include <stdlib.h>
#include <stdio.h>

#define TABLE_SIZE 88179840

unsigned long long nodes = 0;

void search(Cube *cube, int last_turn_face, int depth, uint8_t *table) {

    if(depth == 11) {
        return;
    }

    if(depth < 5)
        printf("%llu nodes explored\n", nodes);

    for(int face = 0; face < 6; face++) {

        if(face == last_turn_face) continue;
        if(last_turn_face == FACE_D && face == FACE_U ||
           last_turn_face == FACE_R && face == FACE_L ||
           last_turn_face == FACE_B && face == FACE_B) continue;
        
        for(int degree = 0; degree < 3; degree++) {
            Cube next = *cube;
            do_move(&next, face, degree);
            nodes++;
            int coord = get_corner_coordinate(&next);
            if(table[coord] <= depth) {
                continue;
            }
            table[coord] = depth;
            search(&next, face, depth + 1, table);
        }

    }

}

void generate_pruning_table() {

    uint8_t *table = malloc(TABLE_SIZE);
    for(int i = 0; i < TABLE_SIZE; i++) {
        table[i] = 0xff;
    }

    Cube cube = create_solved_cube();
    search(&cube, -1, 0, table);

    FILE *fp = fopen("corners.prune", "wb");
    fwrite(table, 1, TABLE_SIZE, fp);
    fclose(fp);

}

int main(int argc, char *argv[]) {
    generate_pruning_table();
}