#include "common.h"

int main (int argc, char ** argv) {
    int cups [1000001];

    intptr_t minlabel = INT64_MAX;
    intptr_t maxlabel = INT64_MIN;

    char buf [BUFSIZ];
    char * s = fgets(&(buf[0]), sizeof(buf), stdin);
    int previous = 0;
    while (isdigit(*s)) {
        int digit = *(s++)-'0';
        cups[previous] = digit;
        minlabel = MIN(minlabel, digit);
        maxlabel = MAX(maxlabel, digit);
        previous = digit;
    }

    while (maxlabel < 1000000) {
        cups[previous] = ++maxlabel;
        previous = maxlabel;
    }

    cups[previous] = cups[0];

    int current = cups[0];
    for (long int move = 1; move <= 10000000; ++move) {
        int removed [3];
        int remove = cups[current];
        for (int i = 0; i < 3; ++i) {
            removed[i] = remove;
            remove = cups[remove];
        }
        cups[current] = remove;

        int dest = (current == minlabel) ? maxlabel : current - 1;
        while (removed[0] == dest || removed[1] == dest || removed[2] == dest) {
            dest -= 1;
            if (dest < minlabel) {
                dest = maxlabel;
            }
        }

        int tmp = cups[dest];
        cups[dest] = removed[0];
        cups[removed[2]] = tmp;

        current = cups[current];
    }

    long int answer = (long int)cups[1] * (long int)cups[cups[1]];
    DISP(answer);
}
