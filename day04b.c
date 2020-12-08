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

    if (strlen(p->byr) != 4) { return 0; }
    int byr = atoi(p->byr);
    if (!(byr >= 1920 && byr <= 2002)) { return 0; }

    if (strlen(p->iyr) != 4) { return 0; }
    int iyr = atoi(p->iyr);
    if (!(iyr >= 2010 && iyr <= 2020)) { return 0; }

    if (strlen(p->eyr) != 4) { return 0; }
    int eyr = atoi(p->eyr);
    if (!(eyr >= 2020 && eyr <= 2030)) { return 0; }

    char * unit_str = NULL;
    long hgt = strtol(p->hgt, &unit_str, 10);
    if (!strcmp(unit_str, "cm")) {
        if (!(hgt >= 150 && hgt <= 193)) { return 0; }
    } else if (!strcmp(unit_str, "in")) {
        if (!(hgt >= 59 && hgt <= 76)) { return 0; }
    } else {
        return 0;
    }

    if (strlen(p->hcl) != 7) { return 0; }
    if (p->hcl[0] != '#') { return 0; }
    for (int i = 1; i < 7; ++i) {
        if (!isxdigit(p->hcl[i])) { return 0; }
    }

    if (!(!strcmp(p->ecl, "amb") || !strcmp(p->ecl, "blu") || !strcmp(p->ecl, "brn") || !strcmp(p->ecl, "gry") || !strcmp(p->ecl, "grn") || !strcmp(p->ecl, "hzl") || !strcmp(p->ecl, "oth"))) { return 0; }

    if (strlen(p->pid) != 9) { return 0; }
    for (int i = 0; i < 9; ++i) {
        if (!isdigit(p->pid[i])) { return 0; }
    }

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

    DISP(n_valid);
}
