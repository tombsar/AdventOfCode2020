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
    intptr_t x;
    intptr_t y;
    intptr_t dx;
    intptr_t dy;
} ShipState_t;

void shipstate_init (struct ShipState * ship) {
    ship->x = 0;
    ship->y = 0;
    ship->dx = 10;
    ship->dy = 1;
}

void runInstruction (struct ShipState * ship, struct Instruction const * inst) {
    switch (inst->action) {
        case ACT_N: {
            ship->dy += inst->distance;
        } break;
        case ACT_S: {
            ship->dy -= inst->distance;
        } break;
        case ACT_E: {
            ship->dx += inst->distance;
        } break;
        case ACT_W: {
            ship->dx -= inst->distance;
        } break;
        case ACT_L: {
            ASSERT((inst->distance % 90) == 0);
            for (intptr_t n = ((inst->distance / 90) % 4); n > 0; --n) {
                intptr_t dx = -ship->dy;
                intptr_t dy = ship->dx;
                ship->dx = dx;
                ship->dy = dy;
            }
        } break;
        case ACT_R: {
            ASSERT((inst->distance % 90) == 0);
            for (intptr_t n = ((inst->distance / 90) % 4); n > 0; --n) {
                intptr_t dx = ship->dy;
                intptr_t dy = -ship->dx;
                ship->dx = dx;
                ship->dy = dy;
            }
        } break;
        case ACT_F: {
            ship->x += ship->dx * inst->distance;
            ship->y += ship->dy * inst->distance;
        } break;
    }
}

intptr_t manhattan (intptr_t x, intptr_t y) {
    return ABS(x)+ABS(y);
}

int main (int argc, char ** argv) {
    struct ShipState ship;
    shipstate_init(&ship);
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
