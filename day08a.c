#include "common.h"

typedef enum Operation {
    OP_NOP,
    OP_ACC,
    OP_JMP
} Operation;

typedef struct Instruction {
    Operation op;
    int arg;
} Instruction_t;

int runProgram (size_t n_instructions, Instruction_t const * instructions) {
    int accumulator = 0;
    size_t pc = 0;
    _Bool * run = calloc(n_instructions, sizeof(_Bool));
    do {
        ASSERT(pc < n_instructions);
        if (run[pc]) {
            break;
        }
        run[pc] = 1;
        switch (instructions[pc].op) {
            case OP_NOP: {
                ++pc;
            } break;
            case OP_ACC: {
                accumulator += instructions[pc].arg;
                ++pc;
            } break;
            case OP_JMP: {
                pc += instructions[pc].arg;
            } break;
        }
    } while (1);
    free(run);
    return accumulator;
}

int main (int argc, char ** argv) {
    size_t instructions_capacity = 256;
    size_t n_instructions = 0;
    Instruction_t * instructions = calloc(instructions_capacity, sizeof(Instruction_t));
    do {
        char op [4];
        int arg;
        int ret = scanf("%3s %i", &(op[0]), &arg);
        if (ret == 2) {
            if (n_instructions == instructions_capacity) {
                instructions_capacity *= 2;
                instructions = realloc(instructions, instructions_capacity*sizeof(Instruction_t));
            }
            Instruction_t * inst = &(instructions[n_instructions++]);
            if (!strcmp(op, "nop")) {
                inst->op = OP_NOP;
            } else if (!strcmp(op, "acc")) {
                inst->op = OP_ACC;
            } else if (!strcmp(op, "jmp")) {
                inst->op = OP_JMP;
            } else {
                ERROR("Unknown operation \"%s\"\n", op);
            }
            inst->arg = arg;
        } else if (ret == EOF) {
            break;
        } else {
            ERROR("scanf failed");
        }
    } while (1);

    DISP(runProgram(n_instructions, instructions));
}