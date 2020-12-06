#include "common.h"
#include "stringview.h"

size_t count_yes_answers (int * a, int n_group_members) {
    size_t count = 0;
    for (size_t i = 0; i < 26; ++i) {
        if (a[i] == n_group_members) {
            ++count;
        }
    }
    return count;
}

int main (int argc, char ** argv) {
    size_t total = 0;
    int n_group_members = 0;
    int responses [26];
    memset(responses, 0, sizeof(responses));
    do {
        char buf [1024];
        char * s = fgets(buf, sizeof(buf), stdin);
        if (s == NULL) {
            total += count_yes_answers(responses, n_group_members);
            n_group_members = 0;
            memset(responses, 0, sizeof(responses));
            break;
        }

        StringView_t line = sv_view_c_string(s);

        sv_eat_spaces(&line);

        if (sv_is_empty(&line)) {
            total += count_yes_answers(responses, n_group_members);
            n_group_members = 0;
            memset(responses, 0, sizeof(responses));
        } else {
            n_group_members += 1;
            do {
                char c = sv_eat_char(&line);
                ASSERT(c >= 'a' && c <= 'z');
                responses[c-'a'] += 1;
                sv_eat_spaces(&line);
            } while (!sv_is_empty(&line));
        }
    } while (1);

    DISP(total, "%zu");
}
