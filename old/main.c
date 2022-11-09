#include "3x3.h"
#include "pruning.h"
#include <stdlib.h>
#include <stdio.h>

#define TABLE_SIZE 88179840

void generate_pruning_table() {

    uint8_t *table = malloc(TABLE_SIZE);
    for(int i = 0; i < TABLE_SIZE; i++) {
        table[i] = 0xff;
    }

    QueueNode *initial = malloc(sizeof(QueueNode));
    initial->cube = create_solved_cube();
    initial->last_turn_face = -1;
    initial->depth = 0;
    initial->next = NULL;


    QueueNode *head = initial, *tail = initial;

    // keep track of the maximum depth reached for logging
    int depth_reached = 0;

    do {

        int cur_depth = head->depth + 1;
        if(cur_depth > depth_reached) {
            printf("reached depth %d\n", cur_depth);
            depth_reached = cur_depth;
        }

        for(int face = 0; face < 6; face++) {

            if(head->last_turn_face == face ||
               head->last_turn_face == FACE_D && face == FACE_U ||
               head->last_turn_face == FACE_R && face == FACE_L ||
               head->last_turn_face == FACE_B && face == FACE_F)
                continue;

            for(int degree = 0; degree < 3; degree++) {
                
                Cube cube = head->cube;
                do_move(&cube, face, degree);

                // if we've seen this state already, ignore it 
                int coord = get_corner_coordinate(&cube);
                if(table[coord] <= cur_depth) {
                    continue;
                }

                table[coord] = cur_depth;
                
                // enqueue the state
                QueueNode *new = malloc(sizeof(QueueNode));
                new->cube = cube;
                new->last_turn_face = face;
                new->depth = cur_depth;
                new->next = NULL;

                tail->next = new;
                tail = new;

            }
        }

        // dequeue
        void *old_head = head;
        head = head->next;
        free(old_head);

    } while(head != NULL);

    FILE *fp = fopen("corners.prune", "wb");
    fwrite(table, 1, TABLE_SIZE, fp);
    fclose(fp);

}

int main(int argc, char *argv[]) {
    generate_pruning_table();
}