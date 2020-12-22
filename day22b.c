#include "common.h"

typedef struct Deque {
    size_t capacity;
    size_t count;
    ptrdiff_t first;
    intptr_t * values;
} Deque_t;

void deque_init (struct Deque * deque, size_t capacity) {
    deque->capacity = capacity;
    deque->count = 0;
    deque->first = 0;
    if (capacity) {
        deque->values = malloc(sizeof(intptr_t)*capacity);
    } else {
        deque->values = NULL;
    }
}

void deque_copy (struct Deque * dest, struct Deque const * src) {
    deque_init(dest, src->capacity);
    dest->count = src->count;
    dest->first = src->first;
    memcpy(dest->values, src->values, sizeof(intptr_t)*src->capacity);
}

void deque_free (struct Deque * deque) {
    free(deque->values);
    deque->capacity = 0;
    deque->count = 0;
    deque->first = 0;
    deque->values = NULL;
}

void deque_push (struct Deque * deque, intptr_t value) {
    ASSERT(deque->count < deque->capacity);
    ptrdiff_t index = (deque->first + deque->count) % deque->capacity;
    deque->values[index] = value;
    deque->count += 1;
}

intptr_t deque_pop (struct Deque * deque) {
    ASSERT(deque->count);
    intptr_t value = deque->values[deque->first];
    deque->first = (deque->first + 1) % deque->capacity;
    deque->count -= 1;
    return value;
}

struct GameState {
    Deque_t player1;
    Deque_t player2;
};

void gamestate_init (struct GameState * state) {
    deque_init(&(state->player1), 52);
    deque_init(&(state->player2), 52);
}

void gamestate_free (struct GameState * state) {
    deque_free(&(state->player1));
    deque_free(&(state->player2));
}

void gamestate_copy (struct GameState * dest, struct GameState const * src) {
    deque_copy(&(dest->player1), &(src->player1));
    deque_copy(&(dest->player2), &(src->player2));
}

_Bool gamestate_equals (struct GameState const * state0, struct GameState const * state1) {
    if (state0->player1.count != state1->player1.count) {
        return 0;
    }
    if (state0->player2.count != state1->player2.count) {
        return 0;
    }

    for (size_t i = 0; i < state0->player1.count; ++i) {
        ptrdiff_t ind0 = (state0->player1.first + i) % state0->player1.capacity;
        ptrdiff_t ind1 = (state1->player1.first + i) % state1->player1.capacity;
        if (state0->player1.values[ind0] != state1->player1.values[ind1]) {
            return 0;
        }
    }

    for (size_t i = 0; i < state0->player2.count; ++i) {
        ptrdiff_t ind0 = (state0->player2.first + i) % state0->player2.capacity;
        ptrdiff_t ind1 = (state1->player2.first + i) % state1->player2.capacity;
        if (state0->player2.values[ind0] != state1->player2.values[ind1]) {
            return 0;
        }
    }

    return 1;
}

int playGame (struct GameState * state) {
    size_t history_capacity = 64;
    size_t history_count = 0;
    struct GameState * history = malloc(sizeof(struct GameState)*history_capacity);

    do {
        for (size_t i = 0; i < history_count; ++i) {
            if (gamestate_equals(state, &(history[i]))) {
                for (size_t j = 0; j < history_count; ++j) {
                    gamestate_free(&(history[j]));
                }
                return 1;
            }
        }
        if (history_count == history_capacity) {
            history_capacity *= 2;
            history = realloc(history, sizeof(struct GameState)*history_capacity);
        }
        gamestate_copy(&(history[history_count++]), state);

        intptr_t v1 = deque_pop(&(state->player1));
        intptr_t v2 = deque_pop(&(state->player2));

        int winner;
        if (v1 <= (intptr_t)state->player1.count && v2 <= (intptr_t)state->player2.count) {
            struct GameState child_state;
            gamestate_init(&child_state);
            for (intptr_t i = 0; i < v1; ++i) {
                ptrdiff_t ind = (state->player1.first + i) % state->player1.capacity;
                deque_push(&(child_state.player1), state->player1.values[ind]);
            }
            for (intptr_t i = 0; i < v2; ++i) {
                ptrdiff_t ind = (state->player2.first + i) % state->player2.capacity;
                deque_push(&(child_state.player2), state->player2.values[ind]);
            }
            winner = playGame(&child_state);
            gamestate_free(&child_state);
        } else {
            winner = (v1 > v2) ? 1 : 2;
        }
        if (winner == 1) {
            deque_push(&(state->player1), v1);
            deque_push(&(state->player1), v2);
        } else {
            ASSERT(winner == 2);
            deque_push(&(state->player2), v2);
            deque_push(&(state->player2), v1);
        }
    } while (state->player1.count && state->player2.count);

    int winner;
    if (state->player1.count) {
        winner = 1;
    } else {
        ASSERT(state->player2.count);
        winner = 2;
    }

    for (size_t i = 0; i < history_count; ++i) {
        gamestate_free(&(history[i]));
    }

    return winner;
}

intptr_t computeScore (struct Deque * cards) {
    intptr_t multiplier = cards->count;
    intptr_t score = 0;
    do {
        intptr_t value = deque_pop(cards);
        score += value * multiplier;
        --multiplier;
    } while (cards->count);
    return score;
}

int main (int argc, char ** argv) {
    struct GameState state;

    gamestate_init(&state);

    char buf [BUFSIZ];
    char * s = fgets(&(buf[0]), sizeof(buf), stdin);
    ASSERT(!strcmp(s, "Player 1:\n"));
    do {
        s = fgets(&(buf[0]), sizeof(buf), stdin);
        ASSERT(s);
        if (!strcmp(s, "\n")) {
            break;
        }
        long value = strtol(s, NULL, 10);
        deque_push(&(state.player1), value);
    } while (1);

    s = fgets(&(buf[0]), sizeof(buf), stdin);
    ASSERT(!strcmp(s, "Player 2:\n"));
    do {
        s = fgets(&(buf[0]), sizeof(buf), stdin);
        if (!s) {
            break;
        }
        if (!strcmp(s, "\n")) {
            break;
        }
        long value = strtol(s, NULL, 10);
        deque_push(&(state.player2), value);
    } while (1);

    int winner = playGame(&state);
    intptr_t score = computeScore((winner == 1) ? &(state.player1) : &(state.player2));
    DISP(score);
}
