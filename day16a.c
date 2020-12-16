#include "common.h"
#include "stringview.h"

typedef struct Range {
    int min;
    int max;
} Range_t;

_Bool isInRange (struct Range const * range, int x) {
    return (x >= range->min && x <= range->max);
}

typedef struct Rule {
    char const * fieldname;
    struct Range ranges [2];
} Rule_t;

_Bool isValid (struct Rule const * rule, int x) {
    return isInRange(&(rule->ranges[0]), x) || isInRange(&(rule->ranges[1]), x);
}

void printRule (struct Rule const * rule) {
    printf("%s: %i-%i or %i-%i\n", rule->fieldname, rule->ranges[0].min, rule->ranges[0].max, rule->ranges[1].min, rule->ranges[1].max);
}

typedef struct Ticket {
    size_t n_numbers;
    int * numbers;
} Ticket_t;

void printTicket (struct Ticket const * ticket) {
    printf("%i", ticket->numbers[0]);
    for (size_t i = 1; i < ticket->n_numbers; ++i) {
        printf(",%i", ticket->numbers[i]);
    }
    printf("\n");
}

int main (int argc, char ** argv) {
    size_t rules_capacity = 20;
    size_t n_rules = 0;
    Rule_t rules [rules_capacity];

    Ticket_t my_ticket;

    size_t tickets_capacity = 512;
    size_t n_tickets = 0;
    Ticket_t tickets [tickets_capacity];

    char buf [BUFSIZ];
    do {
        char * s = fgets(buf, sizeof(buf), stdin);
        ASSERT(s);

        StringView_t line = sv_view_c_string(s);

        sv_eat_spaces(&line);
        if (sv_is_empty(&line)) {
            break;
        }

        StringView_t fieldname = sv_eat_until(&line, ':');
        char colon = sv_eat_char(&line);
        ASSERT(colon == ':');
        sv_eat_spaces(&line);
        StringView_t range0 = sv_eat_until_space(&line);
        sv_eat_spaces(&line);
        StringView_t or = sv_eat_until_space(&line);
        ASSERT(sv_equals(&or, "or"));
        sv_eat_spaces(&line);
        StringView_t range1 = sv_eat_until_space(&line);
        sv_eat_spaces(&line);
        ASSERT(sv_is_empty(&line));

        ASSERT(n_rules < rules_capacity);
        Rule_t * rule = &(rules[n_rules++]);

        rule->fieldname = sv_make_c_string(&fieldname);

        int ret0 = sscanf(range0.start, "%i-%i", &(rule->ranges[0].min), &(rule->ranges[0].max));
        ASSERT(ret0 == 2);

        int ret1 = sscanf(range1.start, "%i-%i", &(rule->ranges[1].min), &(rule->ranges[1].max));
        ASSERT(ret1 == 2);
    } while (1);

    do {
        char * s = fgets(buf, sizeof(buf), stdin);
        ASSERT(s && !strcmp(s, "your ticket:\n"));

        s = fgets(buf, sizeof(buf), stdin);
        ASSERT(s);

        StringView_t line = sv_view_c_string(s);
        size_t n_commas = sv_count(&line, ',');
        my_ticket.n_numbers = 0;
        my_ticket.numbers = calloc(n_commas+1, sizeof(int));
        do {
            ASSERT(my_ticket.n_numbers < n_commas+1);
            my_ticket.numbers[my_ticket.n_numbers++] = strtol(line.start, (char **)&line.start, 10);
            sv_eat_char(&line);
        } while (!sv_is_empty(&line));
    } while (0);

    do {
        // NB: Skip an expected blank line
        char * s = fgets(buf, sizeof(buf), stdin);
        ASSERT(!strcmp(s, "\n"));
    } while (0);

    do {
        char * s = fgets(buf, sizeof(buf), stdin);
        ASSERT(s && !strcmp(s, "nearby tickets:\n"));

        do {
            s = fgets(buf, sizeof(buf), stdin);
            if (!s) {
                break;
            }

            StringView_t line = sv_view_c_string(s);
            sv_eat_spaces(&line);
            if (sv_is_empty(&line)) {
                break;
            }

            size_t n_commas = sv_count(&line, ',');
            ASSERT(n_tickets < tickets_capacity);
            Ticket_t * ticket = &(tickets[n_tickets++]);
            ticket->n_numbers = 0;
            ticket->numbers = calloc(n_commas+1, sizeof(int));
            do {
                ASSERT(ticket->n_numbers < n_commas+1);
                ticket->numbers[ticket->n_numbers++] = strtol(line.start, (char **)&line.start, 10);
                sv_eat_char(&line);
            } while (!sv_is_empty(&line));
        } while (1);
    } while (0);

    size_t error_rate = 0;
    for (size_t i = 0; i < n_tickets; ++i) {
        struct Ticket const * t = &(tickets[i]);
        for (size_t j = 0; j < t->n_numbers; ++j) {
            int x = t->numbers[j];
            _Bool valid = 0;
            for (size_t k = 0; k < n_rules; ++k) {
                if (isValid(&(rules[k]), x)) {
                    valid = 1;
                    break;
                }
            }
            if (!valid) {
                error_rate += x;
            }
        }
    }
    DISP(error_rate);
}
