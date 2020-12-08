#include "common.h"
#include "stringview.h"
#include "glossary.h"

typedef struct Rule {
    intptr_t type_parent;
    intptr_t type_child;
    int quantity;
} Rule_t;

size_t rules_capacity;
size_t n_rules;
Rule_t * rules;

Glossary_t bag_type_glossary;

size_t countContents (intptr_t type) {
    size_t total = 0;
    for (size_t i = 0; i < n_rules; ++i) {
        if (rules[i].type_parent == type) {
            total += rules[i].quantity * (1 + countContents(rules[i].type_child));
        }
    }
    return total;
}

_Bool canContainDirect (intptr_t parent, intptr_t child) {
    for (size_t i = 0; i < n_rules; ++i) {
        if (rules[i].type_parent == parent && rules[i].type_child == child) {
            return 1;
        }
    }
    return 0;
}

_Bool canContainRecursive (intptr_t parent, intptr_t child) {
    if (canContainDirect(parent, child)) {
        return 1;
    } else {
        for (size_t i = 0; i < n_rules; ++i) {
            if (rules[i].type_parent == parent) {
                if (canContainRecursive(rules[i].type_child, child)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int main (int argc, char ** argv) {
    glossary_init(&bag_type_glossary);

    rules_capacity = 1024;
    n_rules = 0;
    rules = malloc(rules_capacity * sizeof(Rule_t));
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

        intptr_t containing_type = glossary_add(&bag_type_glossary, typ);

        StringView_t contain = sv_eat_until_space(&line);
        ASSERT(sv_equals(&contain, "contain"));
        sv_eat_spaces(&line);

        do {
            StringView_t quantity_str = sv_eat_until_space(&line);
            sv_eat_spaces(&line);
            if (sv_equals(&quantity_str, "no")) {
                break;
            } else {
                int quantity = atoi(quantity_str.start);

                StringView_t typ = sv_eat_until_punctuation(&line);

                do {
                    typ.end -= 1;
                } while (!isspace(*typ.end));
                do {
                    typ.end -= 1;
                } while (isspace(*typ.end));
                typ.end += 1;

                intptr_t type = glossary_add(&bag_type_glossary, typ);

                char c = sv_eat_char(&line);
                ASSERT(c == ',' || c == '.');
                sv_eat_spaces(&line);

                Rule_t rule = {
                    .type_parent = containing_type,
                    .type_child = type,
                    .quantity = quantity
                };

                if (n_rules == rules_capacity) {
                    rules_capacity *= 2;
                    rules = realloc(rules, rules_capacity*sizeof(Rule_t));
                }
                rules[n_rules++] = rule;
            }
        } while (!sv_is_empty(&line));
    } while (1);

    intptr_t shiny_gold_id = glossary_add_c_string(&bag_type_glossary, "shiny gold");

    size_t total = countContents(shiny_gold_id);
    DISP(total);
}
