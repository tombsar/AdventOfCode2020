#include "common.h"
#include "stringview.h"

typedef struct Rule {
    char const * containing_type;
    int quantity;
    char const * type;
} Rule_t;

void printRule (struct Rule const * rule) {
    printf("Bag \"%s\" can contain %i \"%s\"\n", rule->containing_type, rule->quantity, rule->type);
}

size_t countContents (size_t n_rules, Rule_t const * rules, char const * type) {
    size_t total = 0;
    for (size_t i = 0; i < n_rules; ++i) {
        if (!strcmp(rules[i].containing_type, type)) {
            total += rules[i].quantity * (1 + countContents(n_rules, rules, rules[i].type));
        }
    }
    return total;
}

_Bool canContainDirectly (size_t n_rules, Rule_t const * rules, char const * parent, char const * child) {
    for (size_t i = 0; i < n_rules; ++i) {
        if (!strcmp(rules[i].containing_type, parent)) {
            if (!strcmp(rules[i].type, child)) {
                return 1;
            }
        }
    }
    return 0;
}

_Bool canContainIndirectly (size_t n_rules, Rule_t const * rules, char const * parent, char const * child) {
    if (canContainDirectly(n_rules, rules, parent, child)) {
        return 1;
    } else {
        for (size_t i = 0; i < n_rules; ++i) {
            if (!strcmp(rules[i].containing_type, parent)) {
                if (canContainIndirectly(n_rules, rules, rules[i].type, child)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int main (int argc, char ** argv) {
    size_t types_capacity = 256;
    size_t n_types = 0;
    char const ** types = malloc(types_capacity * sizeof(char const *));

    size_t rules_capacity = 256;
    size_t n_rules = 0;
    Rule_t * rules = malloc(rules_capacity * sizeof(Rule_t));
    do {
        char buf [1024];
        char * s = fgets(buf, sizeof(buf), stdin);
        if (s == NULL) {
            break;
        }

        StringView_t line = sv_view_c_string(s);

        sv_eat_spaces(&line);

        StringView_t typ = { .start = line.start, .end = line.start };
        StringView_t wrd = { .start = line.start, .end = line.start };
        do {
            typ.end = wrd.end;
            wrd = sv_eat_until_space(&line);
            sv_eat_spaces(&line);
        } while(!sv_equals(&wrd, "bags"));

        char const * containing_type = sv_make_c_string(&typ);

        {
            int id = -1;
            for (int i = 0; i < (int)n_types; ++i) {
                if (!strcmp(types[i], containing_type)) {
                    id = i;
                    break;
                }
            }
            if (id == -1) {
                if (n_types == types_capacity) {
                    types_capacity *= 2;
                    types = realloc(types, types_capacity*sizeof(char const *));
                }
                types[n_types++] = containing_type;
            }
        }

        StringView_t contain = sv_eat_until_space(&line);
        ASSERT(sv_equals(&contain, "contain"));
        sv_eat_spaces(&line);

        do {
            int quantity;
            StringView_t quantity_str = sv_eat_until_space(&line);
            sv_eat_spaces(&line);
            if (sv_equals(&quantity_str, "no")) {
                quantity = 0;
                break;
            } else {
                quantity = atoi(quantity_str.start);

                StringView_t typ = sv_eat_until_punctuation(&line);

                do {
                    typ.end -= 1;
                } while (!isspace(*typ.end));
                do {
                    typ.end -= 1;
                } while (isspace(*typ.end));
                typ.end += 1;

                char const * type = sv_make_c_string(&typ);
                {
                    int id = -1;
                    for (int i = 0; i < (int)n_types; ++i) {
                        if (!strcmp(types[i], containing_type)) {
                            id = i;
                            break;
                        }
                    }
                    if (id == -1) {
                        if (n_types == types_capacity) {
                            types_capacity *= 2;
                            types = realloc(types, types_capacity*sizeof(char const *));
                        }
                        types[n_types++] = containing_type;
                    }
                }

                char c = sv_eat_char(&line);
                ASSERT(c == ',' || c == '.');
                sv_eat_spaces(&line);

                Rule_t rule = {
                    .containing_type = containing_type,
                    .quantity = quantity,
                    .type = type
                };

                if (n_rules == rules_capacity) {
                    rules_capacity *= 2;
                    rules = realloc(rules, rules_capacity*sizeof(Rule_t));
                }
                rules[n_rules++] = rule;
            }
        } while (!sv_is_empty(&line));
    } while (1);

    size_t total = countContents(n_rules, rules, "shiny gold");
    DISP(total, "%zu");
}
