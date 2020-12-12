#include "common.h"

enum Direction {
    DIR_E,
    DIR_N,
    DIR_W,
    DIR_S
};

enum Action {
    ACT_N,
    ACT_S,
    ACT_E,
    ACT_W,
    ACT_L,
    ACT_R,
    ACT_F
};

typedef struct Instruction {
    enum Action action;
    int distance;
} Instruction_t;

typedef struct ShipState {
    enum Direction facing;
    intptr_t x;
    intptr_t y;
} ShipState_t;

void runInstruction (struct ShipState * ship, struct Instruction const * inst) {
    switch (inst->action) {
        case ACT_N: {
            ship->y += inst->distance;
        } break;
        case ACT_S: {
            ship->y -= inst->distance;
        } break;
        case ACT_E: {
            ship->x += inst->distance;
        } break;
        case ACT_W: {
            ship->x -= inst->distance;
        } break;
        case ACT_L: {
            ASSERT((inst->distance % 90) == 0);
            ship->facing = (ship->facing + (inst->distance/90)) % 4;
        } break;
        case ACT_R: {
            ASSERT((inst->distance % 90) == 0);
            ship->facing = (ship->facing - (inst->distance/90)) % 4;
        } break;
        case ACT_F: {
            switch (ship->facing) {
                case DIR_E: {
                    ship->x += inst->distance;
                } break;
                case DIR_N: {
                    ship->y += inst->distance;
                } break;
                case DIR_W: {
                    ship->x -= inst->distance;
                } break;
                case DIR_S: {
                    ship->y -= inst->distance;
                } break;
            }
        } break;
    }
}

intptr_t manhattan (intptr_t x, intptr_t y) {
    return ABS(x)+ABS(y);
}

int main (int argc, char ** argv) {
    struct ShipState ship = {};
    do {
        char a;
        int d;
        int ret = scanf("%c%i\n", &a, &d);
        if (ret == 2) {
            struct Instruction inst;
            switch (a) {
                case 'N': {
                    inst.action = ACT_N;
                } break;
                case 'S': {
                    inst.action = ACT_S;
                } break;
                case 'E': {
                    inst.action = ACT_E;
                } break;
                case 'W': {
                    inst.action = ACT_W;
                } break;
                case 'L': {
                    inst.action = ACT_L;
                } break;
                case 'R': {
                    inst.action = ACT_R;
                } break;
                case 'F': {
                    inst.action = ACT_F;
                } break;
                default: {
                    ERROR("Invalid action '%c'", a);
                }
            }
            inst.distance = d;

            runInstruction(&ship, &inst);
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    DISP(manhattan(ship.x, ship.y));
}
