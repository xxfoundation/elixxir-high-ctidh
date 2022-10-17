// This is from the Public Domain djbsort-20190516
/* this needs __attribute__((optimize)) >= 1 */
#define int32_MINMAX(a,b) do { \
  register const int32_t big = (a > b ? a : b); \
  register const int32_t small = (a > b ? b : a); \
  a = small; \
  b = big; \
} while (0);

