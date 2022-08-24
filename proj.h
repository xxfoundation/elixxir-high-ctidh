#ifndef proj_h
#define proj_h

#include <string.h>
#include "fp.h"

/* P^1 over fp. */
typedef struct proj {
    fp x;
    fp z;
} proj;

static inline void proj_cswap(proj *P, proj *Q, long long c)
{
  fp_cswap(&P->x,&Q->x,c);
  fp_cswap(&P->z,&Q->z,c);
}

static inline void proj_cmov(proj *P, const proj *Q, long long c)
{
  fp_cmov(&P->x,&Q->x,c);
  fp_cmov(&P->z,&Q->z,c);
}

static inline int proj_equal(proj *A,proj *B)
{
  fp AxBz;
  fp AzBx;
  fp_mul3(&AxBz,&A->x,&B->z);
  fp_mul3(&AzBx,&A->z,&B->x);
  return !memcmp(&AxBz,&AzBx,sizeof AzBx);
}

#endif
