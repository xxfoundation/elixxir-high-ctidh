#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mont.h"
#include "steps.h"
#include "primes.h"

void isog_setup(proj *A,proj *P,proj *K,long long k)
{
  A->x = fp_0;
  A->z = fp_1;

  for (;;) {
    fp_random(&P->x);
    fp_random(&P->z);

    uintbig cof = uintbig_1;
    uintbig_mul3_64(&cof, &cof, 4);
    for (long long i = 0;i < primes_num;++i)
      if (primes[i] != k)
        uintbig_mul3_64(&cof, &cof, primes[i]);

    xMUL_vartime(K,A,0,P,&cof);

    if (memcmp(&K->z, &fp_0, sizeof(fp))) break;
  }

  uintbig cof = uintbig_1;
  uintbig_mul3_64(&cof, &cof, k);
  xMUL_vartime(P,A,0,K,&cof);
  if (memcmp(&P->z, &fp_0, sizeof(fp))) abort();

  fp_random(&P->x);
  fp_random(&P->z);
}

int main()
{
  proj A;
  proj P;
  proj K;

  for (long long lpos = 0;lpos < primes_num;++lpos) {
    long long l = primes[lpos];

    isog_setup(&A,&P,&K,l);
    long long baseline = fp_mulsq_count;
    xISOG_old(&A,&P,&K,l);
    baseline = fp_mulsq_count - baseline;

    long long bestbs = 0;
    long long bestgs = 0;
    long long bestbench = -1;

    for (long long bs = 0;bs <= 32;bs += 2) {
      for (long long gs = 0;;++gs) {
        if (!gs) if (bs) continue;
        if (!bs) if (gs) break;
        if (2*bs*gs > (l-1)/2) break;
        if (gs > 2*bs) continue;
        if (bs > 3*gs) continue;

        steps_override(bs,gs);

        isog_setup(&A,&P,&K,l);
        long long bench = fp_mulsq_count;
        xISOG(&A,&P,1,&K,l);
        bench = fp_mulsq_count - bench;

        if (bestbench < 0 || bench < bestbench) {
          bestbs = bs;
          bestgs = gs;
          bestbench = bench;
        }
      }
    }

    printf("%lld %lld %lld %lld %lld\n",l,bestbs,bestgs,bestbench,baseline);
  }

  return 0;
}
