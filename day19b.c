#include "common.h"
#include "stringview.h"

enum TokenType {
    TOKEN_CHAR,
    TOKEN_NUMBER,
    TOKEN_BAR,
};

struct Token {
    enum TokenType type;
    union {
        char character;
        int number;
    };
};

enum MatchType {
    MATCH_CHAR,
    MATCH_REF
};

struct Match {
    enum MatchType type;
    union {
        char character;
        int rule_id;
    };
};

struct MatchList {
    size_t n_matches;
    struct Match matches [];
};

struct Rule {
    size_t n_options;
    struct MatchList * lists [];
};

struct Rule * rules [256] = {};

size_t matchesRule (int rule_id, StringView_t str) {
    if (rule_id == 0) {
        // Rule 0 is special, because it matches rule 42 at least twice, followed by a non-zero number of rule 31 strictly less than the number of 42s
        size_t n_42 = 0;
        size_t n_31 = 0;
        size_t n_matched = 0;
        size_t n;
        while ((n = matchesRule(42, str)) > 0) {
            n_42 += 1;
            n_matched += n;
            str.start += n;
        }
        if (n_42 < 2) {
            return 0;
        }
        while ((n = matchesRule(31, str)) > 0) {
            n_31 += 1;
            n_matched += n;
            str.start += n;
        }
        if (n_31 && n_31 < n_42) {
            return n_matched;
        }
        return 0;
    } else {
        struct Rule * rule = rules[rule_id];
        for (size_t i = 0; i < rule->n_options; ++i) {
            struct MatchList * list = rule->lists[i];
            _Bool matches_all = 1;
            size_t n_matched = 0;
            StringView_t s = str;
            for (size_t j = 0; j < list->n_matches; ++j) {
                if (sv_is_empty(&s)) {
                    matches_all = 0;
                    break;
                }
                struct Match * match = &(list->matches[j]);
                if (match->type == MATCH_CHAR) {
                    if (s.start[0] == match->character) {
                        n_matched += 1;
                        s.start += 1;
                    } else {
                        matches_all = 0;
                        break;
                    }
                } else {
                    ASSERT(match->type == MATCH_REF);
                    size_t n = matchesRule(match->rule_id, s);
                    if (n) {
                        n_matched += n;
                        s.start += n;
                    } else {
                        matches_all = 0;
                        break;
                    }
                }
            }
            if (matches_all) {
                return n_matched;
            }
        }
    }
    return 0;
}

int main (int argc, char ** argv) {
    do {
        char buf [BUFSIZ];
        char * s = fgets(&(buf[0]), sizeof(buf), stdin);
        ASSERT(s);

        StringView_t line = sv_view_c_string(&(buf[0]));
        sv_eat_spaces(&line);
        if (sv_is_empty(&line)) {
            break;
        }

        size_t tokens_count = 0;
        struct Token tokens [256];

        StringView_t id_str = sv_eat_until(&line, ':');
        ASSERT(*line.start == ':');
        sv_eat_char(&line);
        sv_eat_spaces(&line);
        while (!sv_is_empty(&line)) {
            StringView_t token_str = sv_eat_until_space(&line);
            struct Token tok;
            if (sv_equals(&token_str, "|")) {
                tok.type = TOKEN_BAR;
            } else if (isdigit(*token_str.start)) {
                tok.type = TOKEN_NUMBER;
                tok.number = strtol(token_str.start, NULL, 10);
            } else if (token_str.start[0] == '"') {
                ASSERT(sv_length(&token_str) == 3 && token_str.start[2] == '"');
                tok.type = TOKEN_CHAR;
                tok.character = token_str.start[1];
            }
            ASSERT(tokens_count < ARRAYCOUNT(tokens));
            tokens[tokens_count++] = tok;
            sv_eat_spaces(&line);
        }

        size_t n_options = 1;
        for (size_t i = 0; i < tokens_count; ++i) {
            if (tokens[i].type == TOKEN_BAR) {
                n_options += 1;
            }
        }

        struct Rule * rule = malloc(sizeof(struct Rule) + sizeof(struct MatchList *)*n_options);

        size_t i_start = 0;
        for (size_t i = 0; i < tokens_count; ++i) {
            if (tokens[i].type == TOKEN_BAR) {
                size_t n_matches = i - i_start;
                struct MatchList * list = malloc(sizeof(struct MatchList) + sizeof(struct Match)*n_matches);
                list->n_matches = n_matches;
                for (size_t j = 0; j < n_matches; ++j) {
                    struct Match match;
                    switch (tokens[i_start+j].type) {
                        case TOKEN_NUMBER: {
                            match.type = MATCH_REF;
                            match.rule_id = tokens[i_start+j].number;
                        } break;
                        case TOKEN_CHAR: {
                            match.type = MATCH_CHAR;
                            match.character = tokens[i_start+j].character;
                        } break;
                        default: {
                            ASSERT(0);
                        }
                    }
                    list->matches[j] = match;
                }
                rule->lists[rule->n_options++] = list;
                i_start = i+1;
            }
        }
        {
            size_t n_matches = tokens_count - i_start;
            struct MatchList * list = malloc(sizeof(struct MatchList) + sizeof(struct Match)*n_matches);
            list->n_matches = n_matches;
            for (size_t j = 0; j < n_matches; ++j) {
                struct Match match;
                switch (tokens[i_start+j].type) {
                    case TOKEN_NUMBER: {
                        match.type = MATCH_REF;
                        match.rule_id = tokens[i_start+j].number;
                    } break;
                    case TOKEN_CHAR: {
                        match.type = MATCH_CHAR;
                        match.character = tokens[i_start+j].character;
                    } break;
                    default: {
                        ASSERT(0);
                    }
                }
                list->matches[j] = match;
            }
            rule->lists[rule->n_options++] = list;
        }

        unsigned long id = strtoul(id_str.start, NULL, 10);
        ASSERT(id < ARRAYCOUNT(rules));
        rules[id] = rule;
    } while (1);

    size_t n_matching = 0;
    do {
        char buf [BUFSIZ];
        char * s = fgets(&(buf[0]), sizeof(buf), stdin);
        if (!s) {
            break;
        }

        StringView_t line = sv_view_c_string(&(buf[0]));
        sv_eat_spaces(&line);
        if (sv_is_empty(&line)) {
            break;
        }
        line = sv_eat_until_space(&line);

        size_t n = matchesRule(0, line);
        if (n == sv_length(&line)) {
            n_matching += 1;
        }
    } while (1);
    DISP(n_matching);
}
