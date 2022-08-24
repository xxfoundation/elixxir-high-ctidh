#!/usr/bin/env python3

M = 1
S = 1

from costpoly import tree1,multiprod2,multiprod2_selfreciprocal,multieval_precompute,multieval_postcompute

x2 = S+S
x2DBL = M+M+M+M # but save 1M if affine
xDBL = x2+x2DBL
xADD = M+M+S+S+M+M

def isog(lmax,push,bsgs):
  bs,gs = bsgs

  assert lmax > 0
  assert lmax%2
  lbits = 6
  while lmax>>lbits: lbits += 2

  result = 0

  assert bs >= 0
  assert gs >= 0

  if bs == 0 or gs == 0:
    result += xDBL
    result += ((lmax-5)//2)*xADD
    result += ((lmax-3)//2)*2*M
    result += push*(2*M)
    result += ((lmax-3)//2)*4*M*push
  
  else: # velusqrt
    multiples = set()
    multiples.add(2)
    for j in range(3,2*bs+1,2): multiples.add(j)
    for j in range(4,lmax+1-4*bs*gs,2): multiples.add(j)
    multiples.add(2*bs)
    multiples.add(4*bs)
    for j in range(6*bs,2*bs*(2*gs+1),4*bs): multiples.add(j)
  
    result += len(multiples)*xADD # actually xADD or xDBL

    result += tree1[bs]
    result += gs*(3*S+4*M) # biquad_curve
    result += push*(3*S+M) # biquad_precompute_point
    result += push*gs*6*M # biquad_postcompute_point
    result += push*multiprod2[gs] # pushing point
    result += 2*multiprod2_selfreciprocal[gs] # pushing curve
    result += multieval_precompute[bs][gs*2+1] # reciprocal of root of product tree
    result += 2*(push+1)*multieval_postcompute[bs][gs*2+1] # scaled remainder tree
    result += 2*M*(push+1)*(bs-1) # accumulating multieval results
    result += 2*M*(1+2*push)*((lmax-1)//2-2*bs*gs) # stray points at the end
  
  result += push*(S+S+M+M) # final point evaluation
  result += 2*(S+M+M+(S+S+M)*(lbits//2-1)) # powpow8mod
  return result

def optimize(l,push):
  best,bestbsgs = isog(l,push,(0,0)),(0,0)

  # XXX: precompute more tree1 etc.; extend bs,gs limits
  for bs in range(2,33,2):
    for gs in range(1,2*bs+1):
      if 2*bs*gs > (l-1)//2: break
      if gs >= 32: break
      if bs > 3*gs: continue
      result = isog(l,push,(bs,gs))
      if result < best:
        best,bestbsgs = result,(bs,gs)

  return best,bestbsgs

def test1():
  lmax = 587
  bs = 14
  gs = 10
  push = 1

  lbits = 6
  while lmax>>lbits: lbits += 2

  multiples = set()
  multiples.add(2)
  for j in range(3,2*bs+1,2): multiples.add(j)
  for j in range(4,lmax+1-4*bs*gs,2): multiples.add(j)
  multiples.add(2*bs)
  multiples.add(4*bs)
  for j in range(6*bs,2*bs*(2*gs+1),4*bs): multiples.add(j)

  assert len(multiples) == 37
  assert tree1[bs] == 95
  assert gs*(3*S+4*M)+push*(3*S+M)+push*gs*6*M == 134
  assert push*multiprod2[gs] == 139
  assert 2*multiprod2_selfreciprocal[gs] == 176
  assert multieval_precompute[bs][2*gs+1] == 174
  assert multieval_postcompute[bs][2*gs+1] == 251
  assert 2*M*(push+1)*(bs-1) == 52
  assert 2*M*(1+2*push)*((lmax-1)//2-2*bs*gs) == 78
  assert push*(S+S+M+M)+2*(S+M+M+(S+S+M)*(lbits//2-1)) == 34

def test2():
  assert optimize(587,1) == (2108,(14,10))
  assert isog(587,1,(14,10)) == 2108
  assert isog(587,1,(16,9)) == 2118

  assert tree1[14] == 95
  assert tree1[16] == 117

  assert multiprod2[10] == 139
  assert multiprod2[9] == 118
  assert multiprod2_selfreciprocal[10] == 88
  assert multiprod2_selfreciprocal[9] == 74

  assert multieval_precompute[14][21] == 174
  assert multieval_precompute[16][19] == 170
  assert multieval_postcompute[14][21] == 251
  assert multieval_postcompute[16][19] == 285

def test3():
  primes = (3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,587)
  batchsize = (3, 4, 5, 5, 6, 6, 6, 8, 7, 9, 10, 5)
  batchbound = (13, 19, 20, 20, 20, 20, 20, 20, 17, 17, 17, 5)
  B = len(batchsize)
  batchstart = [sum(batchsize[:j]) for j in range(B)]
  batchstop = [sum(batchsize[:j+1]) for j in range(B)]

  bsgs = [optimize(primes[batchstart[b]],1)[1] for b in range(B)]
  C = [[isog(primes[batchstop[b]-1],push,bsgs[b]) for b in range(B)] for push in (0,1,2)]
  assert C[0] == [34, 82, 170, 250, 368, 412, 532, 653, 720, 875, 1034, 1860, ]
  assert C[1] == [48, 120, 252, 372, 546, 643, 830, 1031, 1138, 1385, 1644, 2898, ]
  assert C[2] == [62, 158, 334, 494, 724, 874, 1128, 1409, 1556, 1895, 2254, 3936, ]

if __name__ == '__main__':
  test1()
  test2()
  test3()
