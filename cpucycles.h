#ifndef cpucycles_h
#define cpucycles_h

static inline unsigned long long cpucycles(void) {
  unsigned long long result;

  asm volatile("rdtsc; shlq $32,%%rdx; orq %%rdx,%%rax"
    : "=a" (result) : : "%rdx");

  return result;
}

#endif
