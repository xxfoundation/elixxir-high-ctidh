#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "steps.h"
#include "elligator.h"
#include "csidh.h"
#include "primes.h"

static void test_iszero(void)
{
  printf("uintbig_iszero\n");
  fflush(stdout);

  uintbig u;
  unsigned char *x = (void *) &u;

  memset(x,0,sizeof u);
  assert(uintbig_iszero(&u));

  for (unsigned long long i = 0;i < 8*sizeof u;++i) {
    memset(x,0,sizeof u);
    x[i/8] = 1<<(i&7);
    assert(!uintbig_iszero(&u));
    for (unsigned long long j = 0;j < 8*sizeof u;++j) {
      memset(x,0,sizeof u);
      x[i/8] = 1<<(i&7);
      x[j/8] = 1<<(j&7);
      assert(!uintbig_iszero(&u));
    }
  }
}

static void test_sqrt(void)
{
  printf("fp_sqrt\n");
  fflush(stdout);

  for (long long loop = 0;loop < 1000;++loop) {
    fp x;
    fp xneg;
    fp x2;
    fp x2neg;
    fp_random(&x);
    fp_sq2(&x2,&x);
    fp_neg2(&xneg,&x);
    fp_neg2(&x2neg,&x2);
    assert(fp_sqrt(&x) != fp_sqrt(&xneg));
    assert(fp_sqrt(&x2));
    assert(!fp_sqrt(&x2neg));
  }
}

// ----- validate_contribution -> validate_cutofforder_v0

// input: curve A (affine)
// input: point P (affine) on curve or twist
// input: i between 0 and primes_num-1
// define l = primes[i]
// output: 1 if [(p+1)/l]P has order l
// output: 0 if [(p+1)/l]P has order 1
// output: -1 otherwise
static int validate_contribution(const fp *P,const fp *A,const long long i)
{
  uintbig cofactor = uintbig_1;
  const proj Aproj = {*A,fp_1};
  const proj Pproj = {*P,fp_1};
  proj Q;

  uintbig_mul3_64(&cofactor,&cofactor,4); // maximal 2-power in p+1
  for (long long j = 0;j < primes_num;++j)
    if (j != i)
      uintbig_mul3_64(&cofactor,&cofactor,primes[j]);
  
  xMUL_vartime(&Q,&Aproj,1,&Pproj,&cofactor);
  if (fp_iszero(&Q.z)) return 0;

  uintbig_set(&cofactor,primes[i]);
  xMUL_vartime(&Q,&Aproj,1,&Q,&cofactor);
  if (fp_iszero(&Q.z)) return 1;

  return -1;
}

// input: curve A (affine)
// input: point P (affine) on curve or twist
// for l in list of primes: define contribution(l) as
//    l if [(p+1)/l]P has order l;
//    1 if [(p+1)/l]P has order 1;
//    abort otherwise.
// now consider products, stopping if abort or primes exhausted:
//   contribution(last l),
//   contribution(last l) contribution(previous l),
//   ...
// output: 1 with order = first product > 4 sqrt(p), no aborts found
// or: 0 with order = last product <= 4 sqrt(p), no aborts anywhere
// or: -1 with order = last non-aborting product <= 4 sqrt(p), found an abort
static int validate_cutofforder_v0(uintbig *order,const fp *P1,const fp *A1)
{
  *order = uintbig_1;

  for (long long i = primes_num - 1; i >= 0; --i)
    switch (validate_contribution(P1,A1,i)) {
      case -1:
        return -1;
      case 1:
        uintbig_mul3_64(order,order,primes[i]);
        uintbig tmp;
        if (uintbig_sub3(&tmp, &uintbig_four_sqrt_p, order))
          return 1; // borrow is set, so 4 sqrt(p) < order
    }

  return 0;
}

// ----- cofactor_multiples -> validate_cutofforder_v1

/* compute [(p+1)/l] P for all l in our list of primes. */
/* divide and conquer is much faster than doing it naively,
 * but uses more memory. */
static void cofactor_multiples(proj *P, const proj *A, long long lower, long long upper)
{
    assert(lower < upper);

    if (upper - lower == 1)
        return;

    long long mid = lower + (upper - lower + 1) / 2;

    uintbig cl = uintbig_1, cu = uintbig_1;
    for (long long i = lower; i < mid; ++i)
        uintbig_mul3_64(&cu, &cu, primes[i]);
    for (long long i = mid; i < upper; ++i)
        uintbig_mul3_64(&cl, &cl, primes[i]);

    xMUL_vartime(&P[mid], A, 1, &P[lower], &cu);
    xMUL_vartime(&P[lower], A, 1, &P[lower], &cl);

    cofactor_multiples(P, A, lower, mid);
    cofactor_multiples(P, A, mid, upper);
}

static int validate_cutofforder_v1(uintbig *order,const fp *P,const fp *A)
{
  const proj Aproj = {*A,fp_1};
  proj A24;
  proj Pproj[primes_num];
  Pproj[0].x = *P;
  Pproj[0].z = fp_1;

  xA24(&A24,&Aproj);
  
  /* maximal 2-power in p+1 */
  xDBL(Pproj,Pproj,&A24,1);
  xDBL(Pproj,Pproj,&A24,1);

  cofactor_multiples(Pproj, &Aproj, 0, primes_num);

  *order = uintbig_1;

  for (long long i = primes_num - 1; i >= 0; --i) {
    if (fp_iszero(&Pproj[i].z))
      continue; // [(p+1)/l]P is zero, so not order l

    uintbig tmp;
    uintbig_set(&tmp, primes[i]);
    xMUL_vartime(&Pproj[i], &Aproj, 1, &Pproj[i], &tmp);

    if (!fp_iszero(&Pproj[i].z))
      return -1; // [p+1]P is nonzero

    uintbig_mul3_64(order, order, primes[i]);

    if (uintbig_sub3(&tmp, &uintbig_four_sqrt_p, order))
      return 1; // borrow is set, so order > 4 sqrt(p)
  }
  return 0;
}

// ----- validate_rec_slow -> validate_cutofforder_v2_slow
// includes slow checks not included in validate_rec

static int validate_rec_slow(proj *P, proj const *A, long long lower, long long upper, uintbig *order)
{
    proj Q;
    proj A24;
    xA24(&A24,A);

    proj T;

    assert(lower < upper);

    if (upper - lower == 1) {
      // now P is [(p+1) / l_lower] times the original random point
      if (fp_iszero(&P->z))
        return 0;

      xMUL_dac(&Q, &A24, 1, P, primes_dac[lower], primes_daclen[lower], primes_daclen[lower]);

      uintbig tmp;
      uintbig_set(&tmp, primes[lower]);
      xMUL_vartime(&T, A, 1, P, &tmp);
      assert(proj_equal(&Q,&T));

      if (!fp_iszero(&Q.z))
        return -1;

      uintbig_mul3_64(order, order, primes[lower]);

      if (uintbig_sub3(&tmp, &uintbig_four_sqrt_p, order))
        return 1;
      return 0;
    }

    long long mid = lower + (upper - lower + 1) / 2;

    Q = *P;
    for (long long i = lower; i < mid; ++i)
      xMUL_dac(&Q,&A24,1,&Q,primes_dac[i],primes_daclen[i],primes_daclen[i]);

    uintbig cu = uintbig_1;
    for (long long i = lower; i < mid; ++i)
        uintbig_mul3_64(&cu, &cu, primes[i]);
    xMUL_vartime(&T, A, 1, P, &cu);
    assert(proj_equal(&Q,&T));

    int result = validate_rec_slow(&Q, A, mid, upper, order);
    if (result) return result;

    Q = *P;
    for (long long i = mid; i < upper; ++i)
      xMUL_dac(&Q,&A24,1,&Q,primes_dac[i],primes_daclen[i],primes_daclen[i]);

    uintbig cl = uintbig_1;
    for (long long i = mid; i < upper; ++i)
        uintbig_mul3_64(&cl, &cl, primes[i]);
    xMUL_vartime(&T, A, 1, P, &cl);
    assert(proj_equal(&Q,&T));

    return validate_rec_slow(&Q, A, lower, mid, order);
}

static int validate_cutofforder_v2_slow(uintbig *order,const fp *P,const fp *A)
{
  const proj Aproj = {*A,fp_1};
  proj Pproj = {*P,fp_1};
  proj A24;

  xA24(&A24,&Aproj);

  /* maximal 2-power in p+1 */
  xDBL(&Pproj,&Pproj,&A24,1);
  xDBL(&Pproj,&Pproj,&A24,1);

  *order = uintbig_1;
  return validate_rec_slow(&Pproj,&Aproj,0,primes_num,order);
}
static void test_validate(void)
{
  printf("validate\n");
  fflush(stdout);

  private_key priv;
  public_key pub;

  pub.A = fp_2;
  assert(!validate(&pub));

  fp_neg1(&pub.A);
  assert(!validate(&pub));

  for (long long loop = 0;loop < 10;++loop) {
    fp_random(&pub.A);
    assert(!validate(&pub));

    for (long long j = 0;j < 10;++j) {
      fp P;
      uintbig order0;
      uintbig order1;
      uintbig order2;
      uintbig order2_slow;
      fp_random(&P);
      int v0 = validate_cutofforder_v0(&order0,&P,&pub.A);
      int v1 = validate_cutofforder_v1(&order1,&P,&pub.A);
      int v2 = validate_cutofforder_v2(&order2,&P,&pub.A);
      int v2_slow = validate_cutofforder_v2_slow(&order2_slow,&P,&pub.A);
      assert(v0 == -1);
      assert(v1 == -1);
      assert(v2 == -1);
      assert(v2_slow == -1);
      assert(!memcmp(&order0,&order1,sizeof order0));
      assert(!memcmp(&order0,&order2,sizeof order0));
      assert(!memcmp(&order0,&order2_slow,sizeof order0));
    }
  }

  csidh_private(&priv);
  assert(csidh(&pub,&base,&priv));
  assert(validate(&pub));

  for (long long j = 0;j < 10;++j) {
    fp P;
    uintbig order0;
    uintbig order1;
    uintbig order2;
    uintbig order2_slow;
    fp_random(&P);
    int v0 = validate_cutofforder_v0(&order0,&P,&pub.A);
    int v1 = validate_cutofforder_v1(&order1,&P,&pub.A);
    int v2 = validate_cutofforder_v2(&order2,&P,&pub.A);
    int v2_slow = validate_cutofforder_v2_slow(&order2_slow,&P,&pub.A);
    assert(v0 == 1);
    assert(v1 == 1);
    assert(v2 == 1);
    assert(v2_slow == 1);
    assert(!memcmp(&order0,&order1,sizeof order0));
    assert(!memcmp(&order0,&order2,sizeof order0));
    assert(!memcmp(&order0,&order2_slow,sizeof order0));
    uintbig tmp;
    assert(uintbig_sub3(&tmp,&uintbig_four_sqrt_p,&order0));
  }

  uintbig_add3(&pub.A.x,&pub.A.x,&uintbig_p);
  assert(!validate(&pub));

  pub.A.x = uintbig_p;
  assert(!validate(&pub));
}

static void curve_setup(proj *A)
{
  private_key priv;
  public_key pub;
  csidh_private(&priv);
  assert(csidh(&pub,&base,&priv));
  fp_random(&A->z);
  fp_mul3(&A->x,&pub.A,&A->z);
}

static void kernel_setup(proj *K,long long k,const proj *A)
{
  uintbig cof = uintbig_1;
  uintbig_mul3_64(&cof, &cof, 4);
  for (long long i = 0;i < primes_num;++i)
    if (primes[i] != k)
      uintbig_mul3_64(&cof, &cof, primes[i]);
      
  for (;;) {
    proj P;
    fp_random(&P.x);
    fp_random(&P.z);
    xMUL_vartime(K,A,0,&P,&cof);
    if (memcmp(&K->z, &fp_0, sizeof(fp))) break;
  }
}

static void test_isog(void)
{
  printf("xISOG\n");
  fflush(stdout);

  proj A,K,P[10];
  proj A1,P1[10]; /* overwritten by xISOG */
  proj A2,P2[10]; /* overwritten by xISOG variants */
  curve_setup(&A);
  for (long long t = 0;t <= 10;++t) {
    for (long long i = 0;i < primes_num;++i) {
      kernel_setup(&K,primes[i],&A);
      for (long long j = 0;j < t;++j) {
        fp_random(&P[j].x);
        fp_random(&P[j].z);
      }
      A1 = A;
      for (long long j = 0;j < t;++j)
        P1[j] = P[j];
      xISOG(&A1,P1,t,&K,primes[i]);

      for (long long matryoshka = 0;matryoshka < 10;++matryoshka) {
        long long ilower = random()%(i+1);
        long long iupper = i+(random()%(primes_num-i));
        A2 = A;
        for (long long j = 0;j < t;++j)
          P2[j] = P[j];
        xISOG_matryoshka(&A2,P2,t,&K,primes[i],primes[ilower],primes[iupper]);
        assert(proj_equal(&A2,&A1));
        for (long long j = 0;j < t;++j)
          assert(proj_equal(&P2[j],&P1[j]));
      }

      for (long long j = 0;j < t;++j) {
        A2 = A;
        P2[j] = P[j];
        xISOG_old(&A2,&P2[j],&K,primes[i]);
        assert(proj_equal(&A2,&A1));
        assert(proj_equal(&P2[j],&P1[j]));
      }
      A = A1;
    }
  }
}

static void test_elligator(void)
{
  printf("elligator\n");
  fflush(stdout);

  proj A;
  proj plusminus[10][2];
  fp X[10][2];

  for (long long curves = 0;curves < 10;++curves) {
    if (curves&1)
      curve_setup(&A);
    else {
      A.x = fp_0;
      fp_random(&A.z);
    }
    for (long long i = 0;i < 10;++i) {
      elligator(&plusminus[i][0],&plusminus[i][1],&A);
      fp_inv(&A.z); fp_mul2(&A.x,&A.z);

      for (long long pm = 0;pm < 2;++pm) {
        fp_inv(&plusminus[i][pm].z);
        fp_mul3(&X[i][pm],&plusminus[i][pm].x,&plusminus[i][pm].z);
        fp T;
        fp_add3(&T,&X[i][pm],&A.x);
        fp_mul2(&T,&X[i][pm]);
        fp_add2(&T,&fp_1);
        fp_mul2(&T,&X[i][pm]);
        assert(fp_sqrt(&T) == !pm);
      }
    }
    for (long long i = 0;i < 10;++i)
      for (long long j = 0;j < 10;++j) {
        assert(memcmp(&X[i][0],&X[j][1],sizeof(fp)));
        if (i != j) {
          assert(memcmp(&X[i][0],&X[j][0],sizeof(fp)));
          assert(memcmp(&X[i][1],&X[j][1],sizeof(fp)));
        }
      }
  }
}

// returns 1 if point is on
// complete twisted Edwards curve ax^2+y^2 = 1+dx^2y^2
// in projective coordinates: (aX^2+Y^2)Z^2 = Z^4+dX^2Y^2
static int twisted_isvalid(const fp P[3],const fp *a,const fp *d)
{
  if (fp_iszero(&P[2])) return 0;
  fp X2; fp_sq2(&X2,&P[0]);
  fp Y2; fp_sq2(&Y2,&P[1]);
  fp Z2; fp_sq2(&Z2,&P[2]);
  fp Z4; fp_sq2(&Z4,&Z2);
  fp left; fp_mul3(&left,&X2,a);
  fp_add2(&left,&Y2);
  fp_mul2(&left,&Z2);
  fp right; fp_mul3(&right,&X2,&Y2);
  fp_mul2(&right,d);
  fp_add2(&right,&Z4);
  return fp_isequal(&left,&right);
}

// returns 1 if point on complete twisted Edwards curve
// is the neutral element (0,1)
static int twisted_iszero(const fp P[3])
{
  if (!fp_iszero(&P[0])) return 0;
  return fp_isequal(&P[1],&P[2]);
}

// returns 1 if two points on complete twisted Edwards curve
// are equal
static int twisted_isequal(const fp P[3],const fp Q[3])
{
  fp P0Q2; fp_mul3(&P0Q2,&P[0],&Q[2]);
  fp P2Q0; fp_mul3(&P2Q0,&P[2],&Q[0]);
  if (!fp_isequal(&P0Q2,&P2Q0)) return 0;
  fp P1Q2; fp_mul3(&P1Q2,&P[1],&Q[2]);
  fp P2Q1; fp_mul3(&P2Q1,&P[2],&Q[1]);
  if (!fp_isequal(&P1Q2,&P2Q1)) return 0;
  return 1;
}

// P3=P1+P2 on complete twisted Edwards curve
// in projective coordinates
static void twisted_add(fp P3[3],const fp P1[3],const fp P2[3],const fp *a,const fp *d)
{
  fp X1 = P1[0];
  fp Y1 = P1[1];
  fp Z1 = P1[2];
  fp X2 = P2[0];
  fp Y2 = P2[1];
  fp Z2 = P2[2];
  // https://hyperelliptic.org/EFD/g1p/auto-code/twisted/projective/addition/add-2008-bbjlp.op3
  fp A; fp_mul3(&A,&Z1,&Z2);
  fp B; fp_sq2(&B,&A);
  fp C; fp_mul3(&C,&X1,&X2);
  fp D; fp_mul3(&D,&Y1,&Y2);
  fp t0; fp_mul3(&t0,&C,&D);
  fp E; fp_mul3(&E,d,&t0);
  fp F; fp_sub3(&F,&B,&E);
  fp G; fp_add3(&G,&B,&E);
  fp t1; fp_add3(&t1,&X1,&Y1);
  fp t2; fp_add3(&t2,&X2,&Y2);
  fp t3; fp_mul3(&t3,&t1,&t2);
  fp t4; fp_sub3(&t4,&t3,&C);
  fp t5; fp_sub3(&t5,&t4,&D);
  fp t6; fp_mul3(&t6,&F,&t5);
  fp X3; fp_mul3(&X3,&A,&t6);
  fp t7; fp_mul3(&t7,a,&C);
  fp t8; fp_sub3(&t8,&D,&t7);
  fp t9; fp_mul3(&t9,&G,&t8);
  fp Y3; fp_mul3(&Y3,&A,&t9);
  fp Z3; fp_mul3(&Z3,&F,&G);
  P3[0] = X3;
  P3[1] = Y3;
  P3[2] = Z3;
}

// Q = nP on complete twisted Edwards curve
// P and Q can overlap
static void twisted_mul(fp Q[3],const fp P[3],long long n,const fp *a,const fp *d)
{
  if (n < 0) {
    twisted_mul(Q,P,-n,a,d);
    fp_neg1(&Q[0]);
    assert(twisted_isvalid(Q,a,d));
    return;
  }
  if (n == 0) {
    Q[0] = fp_0;
    Q[1] = fp_1;
    Q[2] = fp_1;
    assert(twisted_isvalid(Q,a,d));
    return;
  }
  if (n == 1) {
    Q[0] = P[0];
    Q[1] = P[1];
    Q[2] = P[2];
    assert(twisted_isvalid(Q,a,d));
    return;
  }
  fp R[3];
  if (n&1) {
    twisted_mul(R,P,n-1,a,d);
    assert(twisted_isvalid(R,a,d));
    twisted_add(Q,R,P,a,d);
    assert(twisted_isvalid(Q,a,d));
    return;
  }
  twisted_mul(R,P,n/2,a,d);
  assert(twisted_isvalid(R,a,d));
  twisted_add(Q,R,R,a,d);
  assert(twisted_isvalid(Q,a,d));
}

// random point on twisted Edwards curve
// in projective coordinates (but representation not randomized)
static void twisted_random(fp P[3],const fp *a,const fp *d)
{
  for (;;) {
    fp x; fp_random(&x);
    fp x2; fp_sq2(&x2,&x);
    fp num; fp_mul3(&num,a,&x2);
    fp den; fp_mul3(&den,d,&x2);
    fp_sub2(&num,&fp_1);
    fp_sub2(&den,&fp_1);
    fp_inv(&den);
    fp_mul2(&num,&den);
    if (fp_sqrt(&num)) {
      P[0] = x;
      P[1] = num;
      P[2] = fp_1;
      return;
    }
  }
}

static void montx_from_twisted(proj *Q,const fp P[3])
{
  fp Y = P[1];
  fp Z = P[2];
  fp_add3(&Q->x,&Y,&Z);
  fp_sub3(&Q->z,&Y,&Z);
}

static void test_dac(void)
{
  printf("dac\n");
  fflush(stdout);

  proj A;

  for (long long b = 0;b < primes_batches;++b)
    for (long long j = primes_batchstart[b];j < primes_batchstop[b];++j)
      assert(primes_daclen[j] <= primes_batchmaxdaclen[b]);

  for (long long curves = 0;curves < 10;++curves) {
    if (curves)
      curve_setup(&A);
    else {
      A.x = fp_0;
      fp_random(&A.z);
    }

    proj A24;
    xA24(&A24,&A);

    proj A1 = A;
    fp_inv(&A1.z);
    fp_mul2(&A1.x,&A1.z);
    A1.z = fp_1;

    proj A124;
    xA24(&A124,&A1);

    for (long long i = 0;i < primes_num;++i) {
      proj P;
      fp_random(&P.x);
      fp_random(&P.z);

      uintbig l;
      uintbig_set(&l,primes[i]);

      proj Q;
      xMUL_vartime(&Q,&A,0,&P,&l);

      proj Q2;
      for (long long ext = 0;ext < 5;++ext) {
        xMUL_dac(&Q2,&A24,0,&P,primes_dac[i],primes_daclen[i],primes_daclen[i]+ext);
        assert(proj_equal(&Q,&Q2));
      }
    }

    fp A1z2; fp_double2(&A1z2,&A1.z);
    fp a; fp_sub3(&a,&A1.x,&A1z2);
    fp d; fp_add3(&d,&A1.x,&A1z2);
    // twisted Edwards curve ax^2+y^2 = 1+dx^2y^2

    fp asqrt = a;
    fp dsqrt = d;
    assert(fp_sqrt(&asqrt));
    assert(!fp_sqrt(&dsqrt));

    for (long long order = 0;order < 100;++order) {
      fp P[3];

      P[0] = fp_0;
      P[1] = fp_0;
      P[2] = fp_0;
      assert(!twisted_isvalid(P,&a,&d));
      P[0] = fp_0;
      P[1] = fp_1;
      P[2] = fp_1;
      assert(twisted_isvalid(P,&a,&d));
      assert(twisted_isequal(P,P));
      assert(twisted_iszero(P));
      fp_random(&P[0]);
      fp_random(&P[1]);
      fp_random(&P[2]);
      assert(!twisted_isvalid(P,&a,&d));

      twisted_random(P,&a,&d);
      assert(twisted_isvalid(P,&a,&d));
      assert(twisted_isequal(P,P));
      assert(!twisted_iszero(P));

      long long actualorder = 0;
      if (order) {
        long long orderleftover = order;
        if (orderleftover%2 == 0) orderleftover /= 2;
        if (orderleftover%2 == 0) orderleftover /= 2;
        for (long long j = 0;j < primes_num;++j)
          if (orderleftover%primes[j] == 0)
            orderleftover /= primes[j];
        if (orderleftover == 1) {
          for (long long j = 0;j < primes_num;++j)
            if (order%primes[j]) {
              twisted_mul(P,P,primes[j],&a,&d);
              assert(twisted_isvalid(P,&a,&d));
            }
          if (order%4) {
            twisted_mul(P,P,2,&a,&d);
            assert(twisted_isvalid(P,&a,&d));
            if (order%2)
              twisted_mul(P,P,2,&a,&d);
          }
          assert(twisted_isvalid(P,&a,&d));

          fp Q[3];

          for (long long k = 1;k <= order;++k)
            if (!(order%k)) {
              twisted_mul(Q,P,k,&a,&d);
              if (twisted_iszero(Q)) {
                actualorder = k;
                break;
              }
            }

          assert(actualorder);
          assert(!(order%actualorder));

          for (long long k = 0;k <= order;++k) {
            twisted_mul(Q,P,k,&a,&d);
            if (k%actualorder)
              assert(!twisted_iszero(Q));
            else
              assert(twisted_iszero(Q));
          }
        }
      }

      fp Q[3];
      fp_random(&Q[2]);
      fp_mul3(&Q[0],&P[0],&Q[2]);
      fp_mul3(&Q[1],&P[1],&Q[2]);
      fp_mul2(&Q[2],&P[2]);
      assert(twisted_isvalid(Q,&a,&d));
      assert(twisted_isequal(Q,P));

      proj Pmont;
      montx_from_twisted(&Pmont,P);
      proj Qmont;
      montx_from_twisted(&Qmont,Q);
      assert(proj_equal(&Pmont,&Qmont));

      fp mP[201][3];
      proj mPmont[201];

      for (long long m = -100;m <= 100;++m) {
        twisted_mul(mP[100+m],P,m,&a,&d);
        assert(twisted_isvalid(mP[100+m],&a,&d));
      }

      for (long long m = -100;m <= 100;++m) {
        for (long long n = -10;n <= 10;++n) {
          if (m+n < -100) continue;
          if (m+n > 100) continue;
          fp R[3];
          twisted_add(R,mP[100+m],mP[100+n],&a,&d);
          assert(twisted_isvalid(R,&a,&d));
          assert(twisted_isequal(R,mP[100+m+n]));
        }
      }

      for (long long m = -100;m <= 100;++m)
        montx_from_twisted(&mPmont[100+m],mP[100+m]);

      for (long long m = -100;m <= 100;++m) {
        proj R;

        if (m == 2) {
          xDBL(&R,&Pmont,&A24,0);
          assert(proj_equal(&R,&mPmont[100+m]));

          xDBL(&R,&Pmont,&A124,1);
          assert(proj_equal(&R,&mPmont[100+m]));
        }
          
        if (m >= 0) {
          uintbig um; uintbig_set(&um,m);
          xMUL_vartime(&R,&A,0,&Pmont,&um);
          assert(proj_equal(&R,&mPmont[100+m]));
  
          xMUL_vartime(&R,&A1,1,&Pmont,&um);
          assert(proj_equal(&R,&mPmont[100+m]));
  
          xMUL(&R,&A,0,&Pmont,&um,20);
          assert(proj_equal(&R,&mPmont[100+m]));
  
          xMUL(&R,&A1,1,&Pmont,&um,20);
          assert(proj_equal(&R,&mPmont[100+m]));
        }

        for (long long j = 0;j < primes_num;++j)
          if (primes[j] == m) {
            for (long long ext = 0;ext < 5;++ext) {
              // xMUL_dac semantics: order reduction, not necessarily mult
              // if actualorder==0, P is random so should always be mult
              xMUL_dac(&R,&A24,0,&Pmont,primes_dac[j],primes_daclen[j],primes_daclen[j]+ext);
              if (!proj_equal(&R,&mPmont[100+m])) {
                assert(proj_equal(&R,&Pmont));
                assert(actualorder%m);
              }
              xMUL_dac(&R,&A124,1,&Pmont,primes_dac[j],primes_daclen[j],primes_daclen[j]+ext);
              if (!proj_equal(&R,&mPmont[100+m])) {
                assert(proj_equal(&R,&Pmont));
                assert(actualorder%m);
              }
            }
          }
      }
    }
  }
}

/* compute x^3 + Ax^2 + x */
static void montgomery_rhs(fp *rhs, fp const *A, fp const *x)
{
    fp tmp;
    *rhs = *x;
    fp_sq1(rhs);
    fp_mul3(&tmp, A, x);
    fp_add2(rhs, &tmp);
    fp_add2(rhs, &fp_1);
    fp_mul2(rhs, x);
}

/* totally not constant-time. */
static void action_old(public_key *out, public_key const *in, private_key const *priv)
{
    uintbig k[2];
    uintbig_set(&k[0], 4); /* maximal 2-power in p+1 */
    uintbig_set(&k[1], 4); /* maximal 2-power in p+1 */

    uint8_t e[2][primes_num];

    for (int64_t i = 0; i < primes_num; ++i) {

        int8_t t = priv->e[i];

        if (t > 0) {
            e[0][i] = t;
            e[1][i] = 0;
            uintbig_mul3_64(&k[1], &k[1], primes[i]);
        }
        else if (t < 0) {
            e[1][i] = -t;
            e[0][i] = 0;
            uintbig_mul3_64(&k[0], &k[0], primes[i]);
        }
        else {
            e[0][i] = 0;
            e[1][i] = 0;
            uintbig_mul3_64(&k[0], &k[0], primes[i]);
            uintbig_mul3_64(&k[1], &k[1], primes[i]);
        }
    }

    proj A = {in->A, fp_1};

    bool done[2] = {false, false};

    do {

        assert(!memcmp(&A.z, &fp_1, sizeof(fp)));

        proj P;
        fp_random(&P.x);
        P.z = fp_1;

        fp rhs;
        montgomery_rhs(&rhs, &A.x, &P.x);
        bool sign = !fp_sqrt(&rhs);

        if (done[sign])
            continue;

        xMUL_vartime(&P, &A, 0, &P, &k[sign]);

        done[sign] = true;

        for (int64_t i = primes_num-1; i >= 0; --i) {  //changed loop direction

            if (e[sign][i]) {

                uintbig cof = uintbig_1;
                for (int64_t j = i - 1; j >= 0; --j)   //changed loop direction
                    if (e[sign][j])
                        uintbig_mul3_64(&cof, &cof, primes[j]);

                proj K;
                xMUL_vartime(&K, &A, 0, &P, &cof);

                if (memcmp(&K.z, &fp_0, sizeof(fp))) {

                    xISOG(&A, &P, 1, &K, primes[i]);

                    if (!--e[sign][i])
                        uintbig_mul3_64(&k[sign], &k[sign], primes[i]);

                }

            }

            done[sign] &= !e[sign][i];
        }

        fp_inv(&A.z);
        fp_mul2(&A.x, &A.z);
        A.z = fp_1;

    } while (!(done[0] && done[1]));

    out->A = A.x;
}

static void test_nike(void)
{
  printf("nike\n");
  fflush(stdout);

  private_key priv_alice, priv_bob;
  public_key pub_alice, pub_bob;
  public_key shared_alice, shared_bob;
  public_key check;
  bool ok;

  for (long long bs = 0;bs <= 64;bs += 2) {
    for (long long gs = 0;;++gs) {
      if (!gs) if (bs) continue;
      if (!bs) if (gs) break;
      if (2*bs*gs > (primes[primes_num-1]-1)/2) break;
      if (gs > 4*bs) continue;
      if (bs > 4*gs) continue;

      printf("trying alice bs=%lld gs=%lld, bob bs=0 gs=0\n",bs,gs);
      fflush(stdout);

      steps_override(bs,gs);
    
      csidh_private(&priv_alice);
      ok = csidh(&pub_alice, &base, &priv_alice);
      assert(ok);
      action_old(&check,&base,&priv_alice);
      assert(!memcmp(&check,&pub_alice,sizeof pub_alice));
    
      steps_override(0,0);

      csidh_private(&priv_bob);
      ok = csidh(&pub_bob, &base, &priv_bob);
      assert(ok);
      action_old(&check,&base,&priv_bob);
      assert(!memcmp(&check,&pub_bob,sizeof pub_bob));
    
      ok = csidh(&shared_bob, &pub_alice, &priv_bob);
      assert(ok);
      action_old(&check,&pub_alice,&priv_bob);
      assert(!memcmp(&check,&shared_bob,sizeof shared_bob));

      steps_override(bs,gs);
    
      ok = csidh(&shared_alice, &pub_bob, &priv_alice);
      assert(ok);
      action_old(&check,&pub_bob,&priv_alice);
      assert(!memcmp(&check,&shared_alice,sizeof shared_alice));
  
      assert(!memcmp(&shared_alice, &shared_bob, sizeof(public_key)));
    }
  }
}

int main()
{
  test_iszero();
  test_sqrt();
  test_dac();
  test_elligator();
  test_validate();
  test_isog();
  test_nike();
  return 0;
}
