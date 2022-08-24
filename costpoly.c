#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "poly.h"

int main()
{
  printf("tree1 = (0");
  for (long long n = 1;n <= 64;++n) {
    fp P[2*n];
    long long s = poly_tree1size(n);
    fp T[s];
    fp_mulsq_count = 0;
    poly_tree1(T,P,n);
    printf(", %lld",fp_mulsq_count);
  }
  printf(")\n");

  printf("multiprod2 = (0");
  for (long long n = 1;n <= 64;++n) {
    fp T[3*n];
    fp_mulsq_count = 0;
    poly_multiprod2(T,n);
    printf(", %lld",fp_mulsq_count);
  }
  printf(")\n");

  printf("multiprod2_selfreciprocal = (0");
  for (long long n = 1;n <= 64;++n) {
    fp T[3*n];
    fp_mulsq_count = 0;
    poly_multiprod2_selfreciprocal(T,n);
    printf(", %lld",fp_mulsq_count);
  }
  printf(")\n");

  for (long long post = 0;post < 2;++post) {
    if (post)
      printf("multieval_postcompute = {\n");
    else
      printf("multieval_precompute = {\n");
    for (long long n = 2;n <= 32;n += 2) {
      fp P[2*n];
      long long s = poly_tree1size(n);
      fp T[s];
      poly_tree1(T,P,n);
      printf("  %lld:{",n);
      for (long long flen = 1;flen <= 64;flen += 2) {
        fp v[n];
        fp f[flen];
        long long t = poly_multieval_precomputesize(n,flen);
        fp precomp[t];
        fp_mulsq_count = 0;
        poly_multieval_precompute(precomp,n,flen,P,T);
        if (post) {
          fp_mulsq_count = 0;
          poly_multieval_postcompute(v,n,f,flen,P,T,precomp);
        }
        printf("%lld:%lld,",flen,fp_mulsq_count);
      }
      printf("},\n");
    }
    printf("}\n");
  }

  return 0;
}
