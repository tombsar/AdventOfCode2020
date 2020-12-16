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

    for (size_t i = 1; i < n_tickets; ++i) {
        ASSERT(tickets[i].n_numbers == tickets[i-1].n_numbers);
    }
    ASSERT(tickets[0].n_numbers == n_rules);

    for (intptr_t i = n_tickets-1; i >= 0; --i) {
        Ticket_t const * t = &(tickets[i]);
        _Bool ticket_valid = 1;
        for (size_t j = 0; j < n_rules; ++j) {
            int x = t->numbers[j];
            _Bool number_valid = 0;
            for (size_t k = 0; k < n_rules; ++k) {
                if (isValid(&(rules[k]), x)) {
                    number_valid = 1;
                    break;
                }
            }
            if (!number_valid) {
                ticket_valid = 0;
                break;
            }
        }
        if (!ticket_valid) {
            tickets[i] = tickets[n_tickets-1];
            --n_tickets;
        }
    }

    int rule_assignments  [n_rules];
    int field_assignments [n_rules];
    for (size_t i = 0; i < n_rules; ++i) {
        rule_assignments[i] = -1;
        field_assignments[i] = -1;
    }

    size_t n_assigned = 0;
    do {
        for (size_t k = 0; k < n_rules; ++k) {
            if (rule_assignments[k] == -1) {
                Rule_t const * r = &(rules[k]);
                size_t n_possible = 0;
                intptr_t possible_index = -1;
                for (size_t j = 0; j < n_rules; ++j) {
                    if (field_assignments[j] == -1) {
                        _Bool all_valid = 1;
                        for (size_t i = 0; i < n_tickets; ++i) {
                            Ticket_t * t = &(tickets[i]);
                            if (!isValid(r, t->numbers[j])) {
                                all_valid = 0;
                                break;
                            }
                        }
                        if (all_valid) {
                            ++n_possible;
                            possible_index = j;
                        }
                    }
                }
                if (n_possible == 1) {
                    rule_assignments[k] = possible_index;
                    field_assignments[possible_index] = k;
                    ++n_assigned;
                }
            }
        }
    } while (n_assigned < n_rules);

    /*
    for (size_t i = 0; i < n_rules; ++i) {
        printf("Field \"%s\" is index %i\n", rules[i].fieldname, rule_assignments[i]);
    }
    */

    size_t answer = 1;
    for (size_t i = 0; i < n_rules; ++i) {
        if (!strncmp(rules[i].fieldname, "departure", 9)) {
            answer *= my_ticket.numbers[rule_assignments[i]];
        }
    }
    DISP(answer);
}
