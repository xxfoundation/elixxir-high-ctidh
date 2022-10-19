#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "random.h"
#include "crypto_declassify.h"

void test_random_coin(void)
{
  printf("random_coin\n");
  fflush(stdout);

  uint64_t dist[2];
  uint64_t loop;

  for (uint64_t den = 1;den <= 10;++den) {
    for (uint64_t num = 0;num <= den;++num) {
      dist[0] = dist[1] = 0;
      for (loop = 0;loop < 16384*den;++loop) {
        int64_t r = random_coin(num,den);
        assert(r <= 0);
        assert(r >= -1);
        ++dist[-r];
      }
      assert(dist[0]+dist[1] == loop);
      if (num == 0) {
        assert(dist[1] == 0);
      } else if (num == den) {
        assert(dist[1] == loop);
      } else {
        assert(dist[1] > (16384-1536)*num);
        assert(dist[1] < (16384+1536)*num);
      }
    }
  }
}

void test_boundedl1_2(void)
{
  printf("boundedl1_2\n");
  fflush(stdout);

  long long dist[11][11];
  int8_t e[4];
  long long table[6] = {1,5,13,25,41,61};

  for (long long S = 0;S <= 5;++S) {
    for (long long i = 0;i < 11;++i)
      for (long long j = 0;j < 11;++j)
        dist[i][j] = 0;

    for (long long loop = 0;loop < table[S]*16384;++loop) {
      e[0] = loop;
      e[3] = loop;
      random_boundedl1(e+1,2,S, (uintptr_t)e, ctidh_fillrandom_default);
      assert(e[0] == (int8_t) loop);
      assert(e[3] == (int8_t) loop);
      assert(abs(e[1])+abs(e[2]) <= S);
      ++dist[e[1]+S][e[2]+S];
    }

    long long numnonzero = 0;
    long long maxnonzero = 0;
    long long minnonzero = 0;

    for (long long i = 0;i < 11;++i)
      for (long long j = 0;j < 11;++j) {
        long long expected = llabs(i-S)+llabs(j-S)<=S;
        long long D = dist[i][j];
        if (D) {
          if (!numnonzero)
            maxnonzero = minnonzero = D;
          else {
            if (D > maxnonzero) maxnonzero = D;
            if (D < minnonzero) minnonzero = D;
          }
          ++numnonzero;
          assert(expected);
        } else {
          assert(!expected);
        }
      }

    assert(minnonzero >= 16384-1536);
    assert(maxnonzero <= 16384+1536);
  }
}

void test_boundedl1_3(void)
{
  printf("boundedl1_3\n");
  fflush(stdout);

  long long dist[11][11][11];
  int8_t e[5];
  long long table[6] = {1,7,25,63,129,231};

  for (long long S = 0;S <= 5;++S) {
    for (long long i = 0;i < 11;++i)
      for (long long j = 0;j < 11;++j)
        for (long long k = 0;k < 11;++k)
          dist[i][j][k] = 0;

    for (long long loop = 0;loop < table[S]*16384;++loop) {
      e[0] = loop;
      e[4] = loop;
      random_boundedl1(e+1,3,S, (uintptr_t)e, ctidh_fillrandom_default);
      assert(e[0] == (int8_t) loop);
      assert(e[4] == (int8_t) loop);
      assert(abs(e[1])+abs(e[2])+abs(e[3]) <= S);
      ++dist[e[1]+S][e[2]+S][e[3]+S];
    }

    long long numnonzero = 0;
    long long maxnonzero = 0;
    long long minnonzero = 0;

    for (long long i = 0;i < 11;++i)
      for (long long j = 0;j < 11;++j)
        for (long long k = 0;k < 11;++k) {
          long long expected = llabs(i-S)+llabs(j-S)+llabs(k-S)<=S;
          long long D = dist[i][j][k];
          if (D) {
            if (!numnonzero)
              maxnonzero = minnonzero = D;
            else {
              if (D > maxnonzero) maxnonzero = D;
              if (D < minnonzero) minnonzero = D;
            }
            ++numnonzero;
            assert(expected);
          } else {
            assert(!expected);
          }
        }

    assert(minnonzero >= 16384-1536);
    assert(maxnonzero <= 16384+1536);
  }
}

#define MAXOUTPUTS 256
#define wMAX 24
#define SMAX 24

double batchkeys(long long w,long long S)
{
  assert(w >= 0);
  assert(w <= wMAX);
  assert(S >= 0);
  assert(S <= SMAX);

  double poly[wMAX+1];
  double newpoly[wMAX+1];

  for (long long j = 0;j <= w;++j) poly[j] = 0;
  poly[0] = 1;
  for (long long i = 0;i < w;++i) {
    for (long long j = 0;j <= w;++j) newpoly[j] = poly[j];
    for (long long j = 0;j < w;++j) newpoly[j+1] += poly[j];
    for (long long j = 0;j <= w;++j) poly[j] = newpoly[j];
  }
  for (long long i = 0;i < S;++i) {
    for (long long j = 0;j <= w;++j) newpoly[j] = poly[j];
    for (long long j = 0;j < w;++j) newpoly[j+1] += 2*poly[j];
    for (long long j = 0;j <= w;++j) poly[j] = newpoly[j];
  }
  return poly[w];
}

double binomial(long long x,long long y)
{
  assert(x >= y);
  assert(y >= 0);
  double result = 1;
  for (long long i = 0;i < y;++i)
    result *= x-i;
  for (long long i = 0;i < y;++i)
    result /= y-i;
  return result;
}

double batchkeys_binomial(long long w,long long S)
{
  double result = 0;
  for (long long k = 0;k <= w && k <= S;++k)
    result += binomial(w,k)*binomial(S,k)*(1<<k);
  return result;
}

void test_batchkeys(void)
{
  printf("batchkeys\n");
  fflush(stdout);
  for (long long w = 0;w <= wMAX;++w)
    for (long long S = 0;S <= wMAX;++S)
      assert(batchkeys(w,S) == batchkeys_binomial(w,S));
  assert(batchkeys(2,0) == 1);
  assert(batchkeys(2,1) == 5);
  assert(batchkeys(2,2) == 13);
  assert(batchkeys(2,3) == 25);
  assert(batchkeys(2,4) == 41);
  assert(batchkeys(2,5) == 61);
  assert(batchkeys(3,0) == 1);
  assert(batchkeys(3,1) == 7);
  assert(batchkeys(3,2) == 25);
  assert(batchkeys(3,3) == 63);
  assert(batchkeys(3,4) == 129);
  assert(batchkeys(3,5) == 231);
}

void test_boundedl1_generic(void)
{
  printf("boundedl1_generic\n");
  fflush(stdout);

  int8_t outputs[MAXOUTPUTS][wMAX];
  long long freq[MAXOUTPUTS];

  for (long long w = 0;w <= wMAX;++w) {
    for (long long S = 0;S <= SMAX;++S) {
      long long expoutputs = batchkeys(w,S);
      if (expoutputs >= MAXOUTPUTS)
        break;

      printf("%lld %lld %lld\n",w,S,expoutputs);
      fflush(stdout);

      long long numoutputs = 0;
      for (long long i = 0;i < MAXOUTPUTS;++i)
        freq[i] = 0;
        
      for (long long loop = 0;loop < 16384*expoutputs;++loop) {
        int8_t e[wMAX];
        random_boundedl1(e,w,S, (uintptr_t)e, ctidh_fillrandom_default);
        crypto_declassify(e,sizeof e);
        int found = 0;
        for (long long i = 0;i < numoutputs;++i) {
          found = 1;
          for (long long j = 0;j < w;++j)
            if (e[j] != outputs[i][j]) {
              found = 0;
              break;
            }
          if (found) {
            ++freq[i];
            break;
          }
        }
        if (!found) {
          assert(numoutputs < expoutputs);
          assert(numoutputs < MAXOUTPUTS);
          for (long long j = 0;j < w;++j)
            outputs[numoutputs][j] = e[j];
          ++freq[numoutputs];
          numoutputs += 1;
        }
      }

      assert(numoutputs == expoutputs);
      for (long long i = 0;i < numoutputs;++i) {
        long long outputsum = 0;
        for (long long j = 0;j < w;++j) {
          assert(outputs[i][j] <= S);
          assert(outputs[i][j] >= -S);
          outputsum += llabs(outputs[i][j]);
        }
        assert(outputsum <= S);
        assert(freq[i] >= 16384-1536);
        assert(freq[i] <= 16384+1536);
      }
    }
  }
}

int main()
{
  test_batchkeys();
  test_random_coin();
  test_boundedl1_2();
  test_boundedl1_3();
  test_boundedl1_generic();
  return 0;
}
