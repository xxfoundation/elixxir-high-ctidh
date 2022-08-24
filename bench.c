#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fp.h"
#include "primes.h"
#include "csidh.h"
#include "cpucycles.h"

void test_nike_1(void)
{
  private_key priv_alice, priv_bob;
  public_key pub_alice, pub_bob;
  public_key shared_alice, shared_bob;
  bool ok = 1;

  csidh_private(&priv_alice);
  csidh_private(&priv_bob);
  ok &= csidh(&pub_alice, &base, &priv_alice);
  ok &= csidh(&pub_bob, &base, &priv_bob);
  ok &= csidh(&shared_bob, &pub_alice, &priv_bob);
  ok &= csidh(&shared_alice, &pub_bob, &priv_alice);
  ok &= !memcmp(&shared_alice, &shared_bob, sizeof(public_key));

  if (!ok) {
    printf("test_nike_1 failure ");
    for (unsigned long long i = 0;i < sizeof(private_key);++i)
      printf("%02x",i[(unsigned char *) &priv_alice]);
    printf(" ");
    for (unsigned long long i = 0;i < sizeof(private_key);++i)
      printf("%02x",i[(unsigned char *) &priv_bob]);
    printf("\n");
    fflush(stdout);
    abort();
  }
}

#define KEYS 65

private_key priv_bob[KEYS];
public_key pub_bob[KEYS];
private_key priv_alice[KEYS];
public_key action_output[KEYS];

int main(int argc,char **argv)
{
  long long target = -1;
  if (argc >= 2) target = atoll(argv[1]);

  for (long long key = 0;key < KEYS;++key) {
    long long cycles0 = cpucycles();
    csidh_private(&priv_alice[key]);
    long long cycles1 = cpucycles();
    csidh_private(&priv_bob[key]);
    long long cycles2 = cpucycles();

    printf("- %lld private cycles %lld\n",key,cycles1-cycles0);
    printf("- %lld private cycles %lld\n",key,cycles2-cycles1);

    action(&pub_bob[key],&base,&priv_bob[key]);
  }

  for (long long key = 0;key < KEYS;++key)
    for (long long key2 = 0;key2 < key;++key2) {
      assert(memcmp(&priv_bob[key],&priv_bob[key2],sizeof(private_key)));
      assert(memcmp(&pub_bob[key],&pub_bob[key2],sizeof(public_key)));
    }

  for (long long key = 0;key < KEYS;++key)
    if (key&1)
      pub_bob[key] = base;

  printf("- - action batchbound");
  for (long long b = 0;b < primes_batches;++b)
    printf(" %lld",primes_batchbound[b]);
  printf("\n");
  printf("- - action pp1");
  long long pos = 0;
  for (long long b = 0;b < primes_batches;++b) {
    long long p = primes[pos];
    printf(" %.12lf",p/(p-1.0));
    pos += primes_batchsize[b];
  }
  printf("\n");

  for (long long loop = 0;loop != target;++loop) {
    test_nike_1();

    for (long long key = 0;key < KEYS;++key) {
      fp_mulsq_count = fp_sq_count = fp_addsub_count = 0;
      long long cycles = cpucycles();
      bool ok = validate(&pub_bob[key]);
      cycles = cpucycles()-cycles;
      printf("%lld %lld validate mulsq %lld sq %lld addsub %lld cycles %lld\n",loop,key,fp_mulsq_count,fp_sq_count,fp_addsub_count,cycles);
      assert(ok);

      for (long long b = 0;b < primes_batches;++b)
        csidh_statsucceeded[b] = csidh_stattried[b] = 0;
      fp_mulsq_count = fp_sq_count = fp_addsub_count = 0;
      cycles = cpucycles();
      action(&action_output[key],&pub_bob[key],&priv_alice[key]);
      cycles = cpucycles()-cycles;
      printf("%lld %lld action mulsq %lld sq %lld addsub %lld cycles %lld\n",loop,key,fp_mulsq_count,fp_sq_count,fp_addsub_count,cycles);
      printf("%lld %lld action stattried",loop,key);
      for (long long b = 0;b < primes_batches;++b) printf(" %lld",csidh_stattried[b]);
      printf("\n");
      for (long long b = 0;b < primes_batches;++b)
        assert(csidh_statsucceeded[b] == primes_batchbound[b]);
    }
    fflush(stdout);
  }

  return 0;
}
