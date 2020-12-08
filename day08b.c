#include "common.h"

typedef enum Operation {
    OP_NOP,
    OP_ACC,
    OP_JMP
} Operation_t;

typedef struct Instruction {
    enum Operation op;
    int arg;
} Instruction_t;

typedef struct Program {
    size_t n_instructions;
    struct Instruction instructions [];
} Program_t;

struct Program * makeProgram (size_t n_instructions, struct Instruction * instructions) {
    struct Program * p = malloc(sizeof(struct Program) + sizeof(struct Instruction)*n_instructions);
    p->n_instructions = n_instructions;
    memcpy(&(p->instructions), instructions, sizeof(struct Instruction)*n_instructions);
    return p;
}

_Bool run (struct Program const * prog, int * result) {
    _Bool terminated_normally = 0;
    int accumulator = 0;
    size_t pc = 0;
    _Bool * already_run = calloc(prog->n_instructions, sizeof(_Bool));
    do {
        if (pc == prog->n_instructions) {
            terminated_normally = 1;
            break;
        }
        if (pc > prog->n_instructions) {
            break;
        }
        if (already_run[pc]) {
            break;
        }
        already_run[pc] = 1;
        switch (prog->instructions[pc].op) {
            case OP_NOP: {
                ++pc;
            } break;
            case OP_ACC: {
                accumulator += prog->instructions[pc].arg;
                ++pc;
            } break;
            case OP_JMP: {
                pc += prog->instructions[pc].arg;
            } break;
        }
    } while (1);
    free(already_run);

    if (terminated_normally) {
        if (result) {
            *result = accumulator;
        }
    }
    return terminated_normally;
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

    struct Program * program = makeProgram(n_instructions, instructions);
    free(instructions);

    for (size_t i = 0; i < program->n_instructions; ++i) {
        if (program->instructions[i].op == OP_NOP || program->instructions[i].op == OP_JMP) {
            Instruction_t tmp = program->instructions[i];
            if (program->instructions[i].op == OP_NOP) {
                program->instructions[i].op = OP_JMP;
            } else {
                program->instructions[i].op = OP_NOP;
            }
            int result;
            if (run(program, &result)) {
                DISP(result);
                break;
            }
            program->instructions[i] = tmp;
        }
    }
}
