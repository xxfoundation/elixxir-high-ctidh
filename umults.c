#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "mont.h"
#include "poly.h"

int main()
{
  long long t = fp_mulsq_count;

  for (long long alen = 0;alen < 20;++alen) {
    fp a[alen];
    for (long long blen = 0;blen < 20;++blen) {
      fp b[blen];
      long long ablen = alen+blen-1;
      if (!alen) ablen = 0;
      if (!blen) ablen = 0;
      for (long long i = 0;i < alen;++i) fp_random(&a[i]);
      for (long long i = 0;i < blen;++i) fp_random(&b[i]);
      fp ab[ablen];
      for (long long i = 0;i < ablen;++i) fp_random(&ab[i]);

      t = fp_mulsq_count;
      poly_mul(ab,a,alen,b,blen);
      printf("poly_mul %lld %lld %lld\n",alen,blen,fp_mulsq_count-t);

      for (long long i = 0;i < ablen;++i) {
        fp t = fp_0;
	for (long long j = 0;j < ablen;++j) {
	  if (j >= 0 && j < alen && i-j >= 0 && i-j < blen) {
	    fp u;
	    fp_mul3(&u,&a[j],&b[i-j]);
	    fp_add2(&t,&u);
	  }
	}
        assert(!memcmp(&ab[i],&t,sizeof(fp)));
      }

      for (long long clen = 0;clen <= ablen;++clen) {
        fp c[clen];
	for (long long i = 0;i < clen;++i) fp_random(&c[i]);
        t = fp_mulsq_count;
	poly_mul_low(c,clen,a,alen,b,blen);
	if (clen >= alen && clen >= blen)
          printf("poly_mul_low %lld %lld %lld %lld\n",clen,alen,blen,fp_mulsq_count-t);
	for (long long i = 0;i < clen;++i)
	  assert(!memcmp(ab,c,clen*sizeof(fp)));
      }
      for (long long cstart = 0;cstart <= ablen;++cstart) {
	long long clen = ablen-cstart;
	fp c[clen];
	for (long long i = 0;i < clen;++i) fp_random(&c[i]);
        t = fp_mulsq_count;
	poly_mul_high(c,cstart,a,alen,b,blen);
        printf("poly_mul_high %lld %lld %lld %lld\n",cstart,alen,blen,fp_mulsq_count-t);
	for (long long i = 0;i < clen;++i)
	  assert(!memcmp(ab+cstart,c,clen*sizeof(fp)));
      }
      for (long long cstart = 0;cstart <= ablen;++cstart) {
        for (long long clen = 0;clen <= ablen-cstart;++clen) {
	  fp c[clen];
	  for (long long i = 0;i < clen;++i) fp_random(&c[i]);
          t = fp_mulsq_count;
	  poly_mul_mid(c,cstart,clen,a,alen,b,blen);
          printf("poly_mul_mid %lld %lld %lld %lld %lld\n",cstart,clen,alen,blen,fp_mulsq_count-t);
	  for (long long i = 0;i < clen;++i)
	    assert(!memcmp(ab+cstart,c,clen*sizeof(fp)));
	}
      }

      for (long long i = 0;i < alen;++i) fp_random(&a[i]);
      for (long long i = 0;i < blen;++i) fp_random(&b[i]);
      for (long long i = 0;i < alen;++i)
        if (i > alen-1-i)
          a[i] = a[alen-1-i];
      for (long long i = 0;i < blen;++i)
        if (i > blen-1-i)
          b[i] = b[blen-1-i];
      for (long long i = 0;i < ablen;++i) fp_random(&ab[i]);
      t = fp_mulsq_count;
      poly_mul_selfreciprocal(ab,a,alen,b,blen);
      printf("poly_mul_selfreciprocal %lld %lld %lld\n",alen,blen,fp_mulsq_count-t);

      for (long long i = 0;i < ablen;++i) {
        fp t = fp_0;
	for (long long j = 0;j < ablen;++j) {
	  if (j >= 0 && j < alen && i-j >= 0 && i-j < blen) {
	    fp u;
	    fp_mul3(&u,&a[j],&b[i-j]);
	    fp_add2(&t,&u);
	  }
	}
        assert(!memcmp(&ab[i],&t,sizeof(fp)));
      }
    }
  }
}
