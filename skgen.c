#include <string.h>
#include <assert.h>

#include "csidh.h"
#include "primes.h"
#include "random.h"

void csidh_private_withrng(private_key *priv, ctidh_fillrandom rng_callback)
{
  memset(&priv->e, 0, sizeof(priv->e));
  long long pos = 0;
  for (long long b = 0;b < primes_batches;++b) {
    long long w = primes_batchsize[b];
    long long S = primes_batchbound[b];
    (void) rng_callback;
    random_boundedl1(priv->e + pos,w,S, (uintptr_t)priv->e, rng_callback);
    pos += w;
  }
  assert(pos <= primes_num);
}

void csidh_private(private_key *priv)
{
	csidh_private_withrng(priv, ctidh_fillrandom_default);
}
