#include <string.h>
#include <assert.h>

#include "csidh.h"
#include "primes.h"
#include "random.h"

void csidh_private(private_key *priv)
{
  memset(&priv->e, 0, sizeof(priv->e));
  long long pos = 0;
  for (long long b = 0;b < primes_batches;++b) {
    long long w = primes_batchsize[b];
    long long S = primes_batchbound[b];
    random_boundedl1(priv->e + pos,w,S);
    pos += w;
  }
  assert(pos <= primes_num);
}
