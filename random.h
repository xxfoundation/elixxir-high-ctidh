#ifndef RANDOM_H
#define RANDOM_H

#include <inttypes.h>
#include "random_namespace.h"

// set up e[0]..e[w-1] having l1 norm at most S, assuming S<128, w<128
void random_boundedl1(int8_t *e,long long w,long long S);

// return -1 with probability num/den, 0 with probability 1-num/den
// assuming 0 <= num <= den, 0 < den < 2^63
int64_t random_coin(uint64_t num,uint64_t den);

#endif
