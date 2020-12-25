#include "common.h"
#include "deque.h"

int main (int argc, char ** argv) {
    Deque_t player1;
    Deque_t player2;

    deque_init(&player1, 64);
    deque_init(&player2, 64);

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
        deque_push(&player1, value);
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
        deque_push(&player2, value);
    } while (1);

    do {
        intptr_t v1 = deque_pop(&player1);
        intptr_t v2 = deque_pop(&player2);
        if (v1 > v2) {
            deque_push(&player1, v1);
            deque_push(&player1, v2);
        } else {
            deque_push(&player2, v2);
            deque_push(&player2, v1);
        }
    } while (player1.count && player2.count);

    Deque_t * winner = NULL;
    if (player1.count) {
        winner = &(player1);
    } else {
        ASSERT(player2.count);
        winner = &(player2);
    }

    intptr_t multiplier = winner->count;
    intptr_t score = 0;
    do {
        intptr_t value = deque_pop(winner);
        score += value * multiplier;
        --multiplier;
    } while (winner->count);
    DISP(score);
}
