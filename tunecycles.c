#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mont.h"
#include "steps.h"
#include "cpucycles.h"
#include "primes.h"

int comparelonglong(const void *uptr,const void *vptr)
{
  long long u = *(const long long *) uptr;
  long long v = *(const long long *) vptr;
  if (u < v) return -1;
  if (u > v) return 1;
  return 0;
}

long long median(long long *x,long long xlen)
{
  if (xlen <= 0) return 0;
  qsort(x,xlen,sizeof(long long),comparelonglong);
  if (xlen&1) return x[xlen/2];
  return (x[xlen/2-1]+x[xlen/2])/2;
}

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

#define TIMINGS 31

int main()
{
  proj A[TIMINGS];
  proj P[TIMINGS];
  proj K[TIMINGS];
  long long baseline[TIMINGS];
  long long bench[TIMINGS];

  for (long long lpos = 0;lpos < primes_num;++lpos) {
    long long l = primes[lpos];

    for (long long t = 0;t < TIMINGS;++t)
      isog_setup(&A[t],&P[t],&K[t],l);
    for (long long t = 0;t < TIMINGS;++t) {
      baseline[t] = cpucycles();
      xISOG_old(&A[t],&P[t],&K[t],l);
      baseline[t] = cpucycles() - baseline[t];
    }
    long long baselinemedian = median(baseline,TIMINGS);

    long long bestbs = 0;
    long long bestgs = 0;
    long long bestbenchmedian = -1;

    for (long long bs = 0;bs <= 32;bs += 2) {
      for (long long gs = 0;;++gs) {
        if (!gs) if (bs) continue;
        if (!bs) if (gs) break;
        if (2*bs*gs > (l-1)/2) break;
        if (gs > bs*2) continue;
        if (bs > gs*3) continue;

        steps_override(bs,gs);

        for (long long t = 0;t < TIMINGS;++t)
          isog_setup(&A[t],&P[t],&K[t],l);

        for (long long t = 0;t < TIMINGS;++t) {
          bench[t] = cpucycles();
          xISOG(&A[t],&P[t],1,&K[t],l);
          bench[t] = cpucycles() - bench[t];
        }
        /* XXX: check for stability, re-run if necessary */

        long long benchmedian = median(bench,TIMINGS);

        if (benchmedian > 0)
          if (bestbenchmedian < 0 || benchmedian < bestbenchmedian) {
            bestbs = bs;
            bestgs = gs;
            bestbenchmedian = benchmedian;
          }
      }
    }

    printf("%lld %lld %lld %lld %lld\n",l,bestbs,bestgs,bestbenchmedian,baselinemedian);
    fflush(stdout);
  }

  return 0;
}
