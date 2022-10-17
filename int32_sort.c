#ifdef HIGHCTIDH_PORTABLE /* begin HIGHCTIDH_PORTABLE */
// This is from the Public Domain release of djbsort-20190516
#include "int32_sort.h"
#define int32 int32_t

#include "int32_minmax.c"

void int32_sort(int32 *x,long long n)
{
  long long top,p,q,r,i;

  if (n < 2) return;
  top = 1;
  while (top < n - top) top += top;

  for (p = top;p > 0;p >>= 1) {
    for (i = 0;i < n - p;++i)
      if (!(i & p))
        int32_MINMAX(x[i],x[i+p]);
    i = 0;
    for (q = top;q > p;q >>= 1) {
      for (;i < n - q;++i) {
        if (!(i & p)) {
          int32 a = x[i + p];
          for (r = q;r > p;r >>= 1)
            int32_MINMAX(a,x[i+r]);
      x[i + p] = a;
    }
      }
    }
  }
}
#else

// This is the original high-ctidh x86_64 sorting code
#include "int32_sort_x86.c"

#endif /* end HIGHCTIDH_PORTABLE */
