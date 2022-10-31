#include "util.h"

bool
has_suffix(char* str, char* suf) {
    int n1 = strlen(str), n2 = strlen(suf);
    if (n1 < n2)
        return false;
    for (int i = 0; i < n2; i++)
       if (str[n1 - i - 1] != suf[n2 - i - 1])
           return false;
    return true;
}

double
time_diff(struct timeval s_t, struct timeval e_t) {
    return e_t.tv_sec - s_t.tv_sec + (e_t.tv_usec - s_t.tv_usec) / 1000000.0;
}
