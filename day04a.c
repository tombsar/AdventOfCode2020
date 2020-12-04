#include "common.h"

typedef struct Passport {
    char * byr;
    char * iyr;
    char * eyr;
    char * hgt;
    char * hcl;
    char * ecl;
    char * pid;
    char * cid;
} Passport_t;

void passport_init (struct Passport * p) {
    p->byr = NULL;
    p->iyr = NULL;
    p->eyr = NULL;
    p->hgt = NULL;
    p->hcl = NULL;
    p->ecl = NULL;
    p->pid = NULL;
    p->cid = NULL;
}

void passport_print (struct Passport const * p) {
    printf("{ byr='%s', iyr='%s', eyr='%s', hgt='%s', hcl='%s', ecl='%s', pid='%s', cid='%s' }\n", p->byr, p->iyr, p->eyr, p->hgt, p->hcl, p->ecl, p->pid, p->cid);
}

_Bool passport_validate (Passport_t const * p) {
    if (!p->byr) return 0;
    if (!p->iyr) return 0;
    if (!p->eyr) return 0;
    if (!p->hgt) return 0;
    if (!p->hcl) return 0;
    if (!p->ecl) return 0;
    if (!p->pid) return 0;
    //if (!p->cid) return 0;

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
            break;
        }

        if (*s == '\n') {
            if (passport_validate(&passport)) {
                ++n_valid;
            }
            passport_init(&passport);
        } else {
            char * p = s;

            while (*p != '\0') {
                while (isspace(*p)) {
                    ++p;
                }

                char * key_start = p++;
                while (*p != ':') {
                    ++p;
                }
                char * key_end = p++;

                char * val_start = p++;
                while (!isspace(*p)) {
                    ++p;
                }
                char * val_end = p++;

                int key_len = key_end - key_start;
                char * key_str = malloc(key_len+1);
                memcpy(key_str, key_start, key_len);
                key_str[key_len] = '\0';

                int val_len = val_end-val_start;
                char * val_str = malloc(val_len+1);
                memcpy(val_str, val_start, val_len);
                val_str[val_len] = '\0';

                if (!strcmp(key_str, "byr")) {
                    passport.byr = val_str;
                } else if (!strcmp(key_str, "iyr")) {
                    passport.iyr = val_str;
                } else if (!strcmp(key_str, "eyr")) {
                    passport.eyr = val_str;
                } else if (!strcmp(key_str, "hgt")) {
                    passport.hgt = val_str;
                } else if (!strcmp(key_str, "hcl")) {
                    passport.hcl = val_str;
                } else if (!strcmp(key_str, "ecl")) {
                    passport.ecl = val_str;
                } else if (!strcmp(key_str, "pid")) {
                    passport.pid = val_str;
                } else if (!strcmp(key_str, "cid")) {
                    passport.cid = val_str;
                } else {
                    ERROR("Unknown key");
                }

                free(key_str);
            }
        }
    } while (1);

    DISP(n_valid, "%zu");
}