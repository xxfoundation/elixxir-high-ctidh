#!/usr/bin/env python3

import costs

# input: list of coeffs of g = g_0+g_1 x+g_2 x^2+...
# input: f
# output: list of coeffs of (f+(1-f)x)g
def polytimeslinear(g,f):
  n = len(g)
  result = [0]*(n+1)
  for s in range(n):
    result[s] += f*g[s]
    result[s+1] += (1-f)*g[s]
  return result

def average(primes,batchsize,batchbound):
  B = len(batchsize)
  assert B == len(batchbound)
  assert sum(batchsize) == len(primes)
  batchstart = [sum(batchsize[:j]) for j in range(B)]
  batchstop = [sum(batchsize[:j+1]) for j in range(B)]

  R = 0
  prsuccess = [[1] for b in range(B)]

  x = {}
  x['AB'] = 0
  x['elligator'] = 0
  x['clear2'] = 0
  for b in range(B):
    x['eachdac',b] = 0
    x['maxdac',b] = 0
    x['isog',0,b] = 0
    x['isog',1,b] = 0
    x['isog',2,b] = 0

  while True:
    # now considering status at beginning of round R
    # one AB per round; rounds numbered from 0

    # for 0 <= s <= R:
    #   prsuccess[b][s] is chance that batch b
    #   had exactly s successes in first R rounds

    prdone = [sum(prsuccess[b][batchbound[b]:]) for b in range(B)]
    # prdone[b] is chance that batch b is done in <=R rounds

    prdoneall = 1
    for b in range(B): prdoneall *= prdone[b]

    prAB = 1-prdoneall
    # we need round R with probability prAB

    x['AB'] += prAB

    for b in range(B):
      # case 1, chance 1-prAB: not AB; forces done[b]
      # case 2, chance 1-prdone[b]: not done[b]; forces not AB
      # case 3, chance prAB-(1-prdone[b]): AB and done[b]

      # in case 3, this AB will do 2x eachdac clearing outside primes
      x['eachdac',b] += 2*(prAB-(1-prdone[b]))

      # in case 2, this AB will do 2x clearing non-selected primes
      x['maxdac',b] += 2*(1-prdone[b])*(batchsize[b]-1)

      # remaining costs depend on targetlen and our position in this AB
      # so figure out distribution of positions

      gfsmaller = [1]
      for a in range(b):
        gfsmaller = polytimeslinear(gfsmaller,prdone[a])
      # gfsmaller is generating function for
      # number of smaller primes in this AB

      gflarger = [1]
      for a in range(b+1,B):
        gflarger = polytimeslinear(gflarger,prdone[a])
      # gfsmaller is generating function for
      # number of larger primes in this AB

      for numsmaller in range(len(gfsmaller)):
        for numlarger in range(len(gflarger)):
          prsituation = (1-prdone[b])*gfsmaller[numsmaller]*gflarger[numlarger]

          targetlen = numsmaller+1+numlarger
          if targetlen <= 3:
            t = numsmaller # 0 1 2
          else:
            # 6 4 3 2 1 0 5 7
            if numlarger == 0:
              t = targetlen-1
            elif numlarger == 1:
              t = 0
            elif numlarger == 2:
              t = targetlen-2
            else:
              t = numlarger-2

          if t == 0:
            x['elligator'] += 2*prsituation
            x['clear2'] += 4*prsituation
            # XXX: can also do these directly from prAB

          # our contribution to kernel points
          # from earlier targets in AB:
          x['maxdac',b] += t*prsituation

          # isogenies:
          if t == targetlen-1:
            push = 0
          elif t == 0:
            push = 1
          else:
            push = 2
          if t == targetlen-2 and targetlen > 2:
            push = 1
          x['isog',push,b] += prsituation*(1-1/primes[batchstart[b]])

          # final mults:
          if t == targetlen-2 and targetlen > 2:
            x['maxdac',b] += prsituation # P0
          elif t < targetlen-1:
            x['maxdac',b] += 2*prsituation # P0, P1

    if prdoneall > 0.999999999:
      return x

    R += 1
    for b in range(B):
      f = 1.0/primes[sum(batchsize[:b])]
      # f is failure probability of batch b
      prsuccess[b] = polytimeslinear(prsuccess[b],f)

def test():
  import sys
  sys.setrecursionlimit(10000)
  for primes,batchsize,batchbound in (
    ( (3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,587),
      (3, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 3, 5, 6, 3),
      (8, 11, 12, 12, 12, 12, 12, 12, 12, 11, 10, 4, 6, 9, 3)
    ),
    ( (3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,587),
      (3, 4, 5, 5, 6, 6, 6, 8, 7, 9, 10, 5),
      (13, 19, 20, 20, 20, 20, 20, 20, 17, 17, 17, 5)
    ),
    ( (3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,983),
      (5, 5, 6, 6, 6, 7, 7, 8, 10, 9, 7, 8, 8, 8, 9, 9, 5, 7),
      (5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 2, 2)
    )
  ):
    x = average(primes,batchsize,batchbound)
    print(costs.strstats(x,'','%.6f',primes,batchsize))

if __name__ == '__main__':
  test()
