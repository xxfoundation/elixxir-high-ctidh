#ifndef FP_H
#define FP_H

#include "uintbig.h"
#include "fp_namespace.h"

/* fp is in the Montgomery domain, so interpreting that
   as an integer should never make sense.
   enable compiler warnings when mixing up uintbig and fp. */
typedef struct fp {
    uintbig x;
} fp;

extern const fp fp_0;
extern const fp fp_1;
extern const fp fp_2;

void fp_cswap(fp *x, fp *y, long long c); /* c is 0 or 1 */
void fp_cmov(fp *x, const fp *y, long long c); /* c is 0 or 1 */

void fp_add2(fp *x, fp const *y);
void fp_sub2(fp *x, fp const *y);
void fp_mul2(fp *x, fp const *y);

void fp_add3(fp *x, fp const *y, fp const *z);
void fp_sub3(fp *x, fp const *y, fp const *z);
void fp_mul3(fp *x, fp const *y, fp const *z);

void fp_sq1(fp *x);
void fp_sq2(fp *x, fp const *y);

extern long long fp_mulsq_count;
extern long long fp_sq_count;
extern long long fp_addsub_count;

static inline void fp_sq1_rep(fp *x,long long n)
{
  while (n > 0) {
    --n;
    fp_sq1(x);
  }
}

static inline void fp_neg1(fp *x)
{
  fp_sub3(x,&fp_0,x);
}

static inline void fp_neg2(fp *x,const fp *y)
{
  fp_sub3(x,&fp_0,y);
}

static inline void fp_double1(fp *x)
{
  fp_add2(x,x);
}

static inline void fp_double2(fp *x,const fp *y)
{
  fp_add3(x,y,y);
}

static inline void fp_quadruple2(fp *x,const fp *y)
{
  fp_double2(x,y);
  fp_double1(x);
}

static inline void fp_quadruple1(fp *x)
{
  fp_double1(x);
  fp_double1(x);
}

static inline long long fp_iszero(const fp *x)
{
  return uintbig_iszero(&x->x);
}

static inline long long fp_isequal(const fp *x,const fp *y)
{
  return uintbig_isequal(&x->x,&y->x);
}

void fp_inv(fp *x);

// if x is a square: replace x by principal sqrt and return 1
// else: return 0
long long fp_sqrt(fp *x);

#include "randombytes.h"
#include "crypto_declassify.h"

static inline void fp_random(fp *x)
{
  for (;;) {
    randombytes(x,sizeof(fp));

    uintbig diff;
    long long accept = uintbig_sub3(&diff,&x->x,&uintbig_p);

    crypto_declassify(&accept,sizeof accept);
    if (accept) return;
  }
}

#endif
