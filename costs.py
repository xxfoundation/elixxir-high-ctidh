#!/usr/bin/env python3

from memoized import memoized
import chain
import costisog

M = 1
S = 1

x2 = S+S
x2DBL = M+M+M+M # but save 1M if affine
xDBL = x2+x2DBL
xADD = M+M+S+S+M+M
clear2 = xDBL

def daccost(n):
  return xDBL+xADD+n*xADD

@memoized
def bigprime(primes):
  p = 4
  for l in primes: p *= l
  p -= 1
  return p

@memoized
def inv(primes):
  p = bigprime(primes)
  invchain = chain.chain2(p-2)
  invchaincost = chain.cost2(invchain)
  return invchaincost[0]*M+invchaincost[1]*S

@memoized
def div(primes):
  return inv(primes)+M

@memoized
def sqrt(primes):
  p = bigprime(primes)
  sqrtchain = chain.chain2((p+1)//4)
  sqrtchaincost = chain.cost2(sqrtchain)
  return sqrtchaincost[0]*M+(sqrtchaincost[1]+1)*S

@memoized
def elligator(primes):
  return S+M+M+M+S+M+M+sqrt(primes)

def dac_search(target,r0,r1,r2,chain,chainlen,best,bestlen):
  if chainlen >= bestlen: return best,bestlen
  if r2 > target: return best,bestlen
  if r2<<(bestlen-1-chainlen) < target: return best,bestlen
  if r2 == target: return chain,chainlen
  chain *= 2
  chainlen += 1
  best,bestlen = dac_search(target,r0,r2,r0+r2,chain+1,chainlen,best,bestlen)
  best,bestlen = dac_search(target,r1,r2,r1+r2,chain,chainlen,best,bestlen)
  return best,bestlen

def dac(target):
  best = None
  bestlen = 0
  while best == None:
    bestlen += 1
    best,bestlen = dac_search(target,1,2,3,0,0,best,bestlen)
  return best,bestlen

@memoized
def daclen(primes):
  return [dac(primes[j])[1] for j in range(len(primes))]

@memoized
def batchstart(batchsize):
  B = len(batchsize)
  return [sum(batchsize[:j]) for j in range(B)]

@memoized
def batchstop(batchsize):
  B = len(batchsize)
  return [sum(batchsize[:j+1]) for j in range(B)]

@memoized
def maxdaclen(primes,batchsize):
  B = len(batchsize)
  return [max(daclen(primes)[j]
              for j in range(batchstart(batchsize)[b],
                             batchstop(batchsize)[b]))
          for b in range(B)]

@memoized
def maxdac(primes,batchsize,b):
  B = len(batchsize)
  M = maxdaclen(primes,batchsize)
  return daccost(M[b])

@memoized
def eachdac(primes,batchsize,b):
  B = len(batchsize)
  D = daclen(primes)
  return sum(daccost(D[j])
             for j in range(batchstart(batchsize)[b],
                            batchstop(batchsize)[b]))

@memoized
def bsgs(primes,batchsize,b):
  return costisog.optimize(primes[batchstart(batchsize)[b]],1)[1]

@memoized
def isog(push,primes,batchsize,b):
  bs,gs = bsgs(primes,batchsize,b)
  return costisog.isog(primes[batchstop(batchsize)[b]-1],push,(bs,gs))

def mults(x,primes,batchsize):
  B = len(batchsize)
  mults = 0
  mults += div(primes)
  mults += x['elligator']*elligator(primes)
  mults += x['clear2']*clear2
  for b in range(B):
    mults += x['maxdac',b]*maxdac(primes,batchsize,b)
    mults += x['eachdac',b]*eachdac(primes,batchsize,b)
    mults += x['isog',0,b]*isog(0,primes,batchsize,b)
    mults += x['isog',1,b]*isog(1,primes,batchsize,b)
    mults += x['isog',2,b]*isog(2,primes,batchsize,b)
  return mults

def strstats(x,prefix,format,primes,batchsize):
  B = len(batchsize)
  result = prefix
  result += 'mults %s ' % (format%mults(x,primes,batchsize))
  result += 'AB %s ' % (format%x['AB'])
  result += 'elligator %s ' % (format%x['elligator'])
  result += 'clear2 %s ' % (format%x['clear2'])
  result += 'isog0 %s ' % ' '.join(format%x['isog',0,b] for b in range(B))
  result += 'isog1 %s ' % ' '.join(format%x['isog',1,b] for b in range(B))
  result += 'isog2 %s ' % ' '.join(format%x['isog',2,b] for b in range(B))
  result += 'maxdac %s ' % ' '.join(format%x['maxdac',b] for b in range(B))
  result += 'eachdac %s ' % ' '.join(format%x['eachdac',b] for b in range(B))
  return result

def test():
  primes = (3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,587)
  batchsize = (3, 4, 5, 5, 6, 6, 6, 8, 7, 9, 10, 5)
  batchbound = (13, 19, 20, 20, 20, 20, 20, 20, 17, 17, 17, 5)
  B = len(batchsize)
  C = [[isog(push,primes,batchsize,b) for b in range(B)] for push in range(3)]
  assert C[0] == [34, 82, 170, 250, 368, 412, 532, 653, 720, 875, 1034, 1860, ]
  assert C[1] == [48, 120, 252, 372, 546, 643, 830, 1031, 1138, 1385, 1644, 2898, ]
  assert C[2] == [62, 158, 334, 494, 724, 874, 1128, 1409, 1556, 1895, 2254, 3936, ]

if __name__ == '__main__':
  test()
