#include "common.h"
#include "stringview.h"

typedef struct Passport {
    char const * byr;
    char const * iyr;
    char const * eyr;
    char const * hgt;
    char const * hcl;
    char const * ecl;
    char const * pid;
    char const * cid;
} Passport_t;

void passport_init (struct Passport * p) {
    memset(p, 0, sizeof(struct Passport));
}

void passport_erase (struct Passport * p) {
    free((void *)p->byr);
    free((void *)p->iyr);
    free((void *)p->eyr);
    free((void *)p->hgt);
    free((void *)p->hcl);
    free((void *)p->ecl);
    free((void *)p->pid);
    free((void *)p->cid);
    passport_init(p);
}

void passport_print (struct Passport const * p) {
    printf("passport: { byr='%s', iyr='%s', eyr='%s', hgt='%s', hcl='%s', ecl='%s', pid='%s', cid='%s' }\n", p->byr, p->iyr, p->eyr, p->hgt, p->hcl, p->ecl, p->pid, p->cid);
}

_Bool passport_validate (struct Passport const * p) {
    if (!p->byr) { return 0; }
    if (!p->iyr) { return 0; }
    if (!p->eyr) { return 0; }
    if (!p->hgt) { return 0; }
    if (!p->hcl) { return 0; }
    if (!p->ecl) { return 0; }
    if (!p->pid) { return 0; }
    //if (!p->cid) { return 0; }

    return 1;
}

int main (int argc, char ** argv) {
    size_t n_valid = 0;

    Passport_t passport;
    passport_init(&passport);
    do {
        char buf [1024];
        char * s = fgets(buf, sizeof(buf), stdin);
        if (s == NULL) {
            if (passport_validate(&passport)) {
                ++n_valid;
            }
            passport_erase(&passport);
            break;
        }

        StringView_t line = sv_view_c_string(s);

        sv_eat_spaces(&line);

        if (sv_is_empty(&line)) {
            if (passport_validate(&passport)) {
                ++n_valid;
            }
            passport_erase(&passport);
        } else {
            do {
                StringView_t key = sv_eat_until(&line, ':');
                ASSERT(!sv_is_empty(&key));

                sv_eat_char(&line);

                StringView_t val = sv_eat_until_space(&line);
                ASSERT(!sv_is_empty(&val));

                sv_eat_spaces(&line);

                if (sv_equals(&key, "byr")) {
                    passport.byr = sv_make_c_string(&val);
                } else if (sv_equals(&key, "iyr")) {
                    passport.iyr = sv_make_c_string(&val);
                } else if (sv_equals(&key, "eyr")) {
                    passport.eyr = sv_make_c_string(&val);
                } else if (sv_equals(&key, "hgt")) {
                    passport.hgt = sv_make_c_string(&val);
                } else if (sv_equals(&key, "hcl")) {
                    passport.hcl = sv_make_c_string(&val);
                } else if (sv_equals(&key, "ecl")) {
                    passport.ecl = sv_make_c_string(&val);
                } else if (sv_equals(&key, "pid")) {
                    passport.pid = sv_make_c_string(&val);
                } else if (sv_equals(&key, "cid")) {
                    passport.cid = sv_make_c_string(&val);
                } else {
                    ERROR("Unknown key \"%s\"", sv_make_c_string(&key));
                }
            } while (!sv_is_empty(&line));
        }
    } while (1);

    DISP(n_valid, "%zu");
}
