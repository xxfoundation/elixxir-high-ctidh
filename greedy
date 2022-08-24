#!/usr/bin/env python3

# sample usage: ./greedy.py 512 256 3 0 2
# CSIDH-512 prime
# >=2^256 keys
# B=3
# force the 0 largest primes to be skipped
# try to use 2 cores

from multiprocessing import Pool

from memoized import memoized
import distmults
import costs
import sys

def printstatus(prefix,cost,N0,m0,numprimes1):
  N = N0 if numprimes1 == 0 else N0+(numprimes1,)
  m = m0 if numprimes1 == 0 else m0+(0,)
  print('%s %.2f %s %s' % (prefix,cost,str(N).replace(' ',''),str(m).replace(' ','')))

def costfunction(primes0,primes1,N0,m0):
  primes = primes0+primes1
  N = N0+(len(primes1),) if len(primes1) > 0 else N0
  m = m0+(0,) if len(primes1) > 0 else m0
  x = distmults.average(primes,N,m)
  return costs.mults(x,primes,N)

@memoized
def batchkeys(x,y):
  poly = [1]
  for i in range(x):
    newpoly = poly+[0]
    for j in range(len(poly)):
      newpoly[j+1] += poly[j]
    poly = newpoly
  for i in range(y):
    newpoly = poly+[0]
    for j in range(len(poly)):
      newpoly[j+1] += 2*poly[j]
    poly = newpoly
  return poly[x]

@memoized
def keys(N,m):
  result = 1
  for s,b in zip(N,m):
    result *= batchkeys(s,b)
  return result

# neighboring_intvec; search upwards in non-b directions
def searchup(minkeyspace,primes0,primes1,N0,m0,cost,b,best):
  if cost >= best[0]:
    return best
  if keys(N0,m0) >= minkeyspace:
    return cost,m0

  B0 = len(N0)

  for c in range(B0):
    if c == b: continue
    upm = list(m0)
    upm[c] += 1
    upm = tuple(upm)
    upcost = costfunction(primes0,primes1,N0,upm)
    best = searchup(minkeyspace,primes0,primes1,N0,upm,upcost,b,best)
  return best

def optimizem(minkeyspace,primes0,primes1,N0,m0=None):
  B0 = len(N0)

  if m0 == None:
    N0 = tuple(N0)
    assert sum(N0) == len(primes0)
  
    z = 1
    while True:
      m0 = tuple([z]*B0)
      if keys(N0,m0) >= minkeyspace: break
      z += 1
  else:
    while keys(N0,m0) < minkeyspace:
      m0 = list(m0)
      m0[0] += 1
      m0 = tuple(m0)

  cost = costfunction(primes0,primes1,N0,m0)

  while True:
    printstatus('searching',cost,N0,m0,len(primes1))
    sys.stdout.flush()

    best = cost,m0
    for b in range(B0):
      if m0[b] == 0: continue
      newm = list(m0)
      newm[b] -= 1
      newm = tuple(newm)
      newcost = costfunction(primes0,primes1,N0,newm)
      best = searchup(minkeyspace,primes0,primes1,N0,newm,newcost,b,best)
    if best == (cost,m0): break
    cost,m0 = best

  return cost,m0

def optimizeNm(minkeyspace,primes0,primes1,B,parallelism=1):
  B0 = B-1 if len(primes1)>0 else B
  N0 = tuple(len(primes0)//B0+(j<len(primes0)%B0) for j in range(B0))
  cost,m0 = optimizem(minkeyspace,primes0,primes1,N0)

  while True:
    best = cost,N0,m0
    variants = []
    for b in range(B0):
      if N0[b] <= 1: continue
      for c in range(B0):
        if c == b: continue
        newsize = list(N0)
        newsize[b] -= 1
        newsize[c] += 1
        newsize = tuple(newsize)
        variants += [(minkeyspace,primes0,primes1,newsize,m0)]
    with Pool(parallelism) as p:
      results = p.starmap(optimizem,variants,chunksize=1)
    for (newcost,newm),(_,_,_,newsize,_) in zip(results,variants):
      if newcost < best[0]:
        best = newcost,newsize,newm
    if best == (cost,N0,m0): break
    cost,N0,m0 = best

  return cost,N0,m0

def doit():
  sys.setrecursionlimit(10000)

  p = {}
  p['512'] = (3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,587)
  p['1024'] = (3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,983)
  p['2048'] = (3,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,1453,1459,3413)

  primes = p['512']
  if len(sys.argv) > 1:
    primes = p[sys.argv[1]]

  minkeyspace = 2**256
  if len(sys.argv) > 2:
    minkeyspace = 2**float(sys.argv[2])

  B = 3
  if len(sys.argv) > 3:
    B = int(sys.argv[3])
  assert B >= 1
  assert B <= len(primes)

  numprimes1 = 0
  if len(sys.argv) > 4:
    numprimes1 = int(sys.argv[4])
  assert 0 <= numprimes1
  if numprimes1 > 0: assert B >= 2

  primes0 = primes[:len(primes)-numprimes1]
  primes1 = primes[len(primes)-numprimes1:]

  parallelism = 1
  if len(sys.argv) > 5:
    parallelism = int(sys.argv[5])

  cost,N0,m0 = optimizeNm(minkeyspace,primes0,primes1,B,parallelism)
  printstatus('output',cost,N0,m0,len(primes1))

if __name__ == '__main__':
  doit()
