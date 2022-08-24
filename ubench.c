#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fp.h"
#include "poly.h"
#include "mont.h"
#include "steps.h"
#include "uintbig.h"
#include "cpucycles.h"
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

#define TIMINGS 7
long long t[TIMINGS];
long long t2[TIMINGS];

int main()
{
  fp a,b,c;
  fp F[100],G[100],H[100];
  proj A[TIMINGS],P[TIMINGS],K[TIMINGS];

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  printf("nothing");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    fp_random(&c);
  }
  printf("fp_random");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    fp_add3(&c,&a,&b);
  }
  printf("fp_add3");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
    fp_add3(&c,&a,&b);
  }
  printf("10xfp_add3");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    fp_sub3(&c,&a,&b);
  }
  printf("fp_sub3");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    fp_sq2(&c,&a);
  }
  printf("fp_sq2");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    fp_mul3(&c,&a,&b);
  }
  printf("fp_mul3");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    poly_mul(H,F,1,G,1);
  }
  printf("poly_mul11");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    poly_mul(H,F,2,G,1);
  }
  printf("poly_mul21");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    poly_mul(H,F,3,G,1);
  }
  printf("poly_mul31");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    poly_mul(H,F,4,G,1);
  }
  printf("poly_mul41");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    poly_mul(H,F,2,G,2);
  }
  printf("poly_mul22");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    poly_mul(H,F,3,G,2);
  }
  printf("poly_mul32");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    poly_mul(H,F,4,G,2);
  }
  printf("poly_mul42");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long i = 0;i < TIMINGS;++i) t[i] = cpucycles();
  for (long long i = 0;i < TIMINGS;++i) {
    t[i] = cpucycles();
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
    fp_mul3(&c,&a,&b);
  }
  printf("10xfp_mul3");
  for (long long i = 1;i < TIMINGS;++i) printf(" %lld",t[i]-t[i-1]);
  printf("\n");

  for (long long lpos = 0;lpos < primes_num;++lpos) {
    for (long long new = 0;new < 2;++new) {
      int stopearly = 0;
      for (long long bs = 0;bs <= 32;bs += 2) {
	if (stopearly) break;
        for (long long gs = 0;;++gs) {
	  if (!gs) if (bs) continue;
	  if (!bs) if (gs) break;
	  if (2*bs*gs > (primes[lpos]-1)/2) break;
	  if (gs > 4*bs) continue;
	  if (bs > 4*gs) continue;
	  if (stopearly) break;
          for (long long i = 0;i < TIMINGS;++i)
            isog_setup(&A[i],&P[i],&K[i],primes[lpos]);
	  if (new) {
            steps_override(bs,gs);
            for (long long i = 0;i < TIMINGS;++i)
              t[i] = cpucycles();
            for (long long i = 0;i < TIMINGS;++i) {
              t[i] = cpucycles();
	      t2[i] = fp_mulsq_count;
              xISOG(&A[i],&P[i],1,&K[i],primes[lpos]);
            }
	  } else {
            for (long long i = 0;i < TIMINGS;++i)
              t[i] = cpucycles();
            for (long long i = 0;i < TIMINGS;++i) {
              t[i] = cpucycles();
	      t2[i] = fp_mulsq_count;
              xISOG_old(&A[i],&P[i],&K[i],primes[lpos]);
            }
	    stopearly = 1;
	  }
          printf("xISOG%lld",primes[lpos]);
	  if (!new) printf("_old");
          printf(" bs=%lld",bs);
          printf(" gs=%lld",gs);
	  printf(" cycles");
          for (long long i = 1;i < TIMINGS;++i)
            printf(" %lld",t[i]-t[i-1]);
          printf("\n");
          printf("xISOG%lld",primes[lpos]);
	  if (!new) printf("_old");
          printf(" bs=%lld",bs);
          printf(" gs=%lld",gs);
	  printf(" mults");
          for (long long i = 1;i < TIMINGS;++i)
            printf(" %lld",t2[i]-t2[i-1]);
          printf("\n");
        }
      }
    }
  }

  return 0;
}
