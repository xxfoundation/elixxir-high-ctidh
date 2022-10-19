#ifndef RANDOM_H
#define RANDOM_H

#include <inttypes.h>
#include <stdint.h>
#include <stddef.h>
#include "random_namespace.h"

typedef void ((ctidh_fillrandom)(
  void *const outbuf,
  const size_t outsz,
  const uintptr_t context));

ctidh_fillrandom ctidh_fillrandom_default;

// set up e[0]..e[w-1] having l1 norm at most S, assuming S<128, w<128
void random_boundedl1(int8_t *e,long long w,long long S, uintptr_t rng_context, ctidh_fillrandom rng_callback);

// return -1 with probability num/den, 0 with probability 1-num/den
// assuming 0 <= num <= den, 0 < den < 2^63
int64_t random_coin(uint64_t num,uint64_t den);

#endif
