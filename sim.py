#!/usr/bin/env python3

import costs
import random
import sys

def trial(primes,batchsize,batchbound):
  B = len(batchsize)
  assert B == len(batchbound)
  assert sum(batchsize) == len(primes)
  batchstart = [sum(batchsize[:j]) for j in range(B)]
  batchstop = [sum(batchsize[:j+1]) for j in range(B)]

  todo = list(batchbound)

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

  def clearnonselected():
    # clear 4 from P:
    x['clear2'] += 2

    # clear primes outside batches from P:
    for b in range(B):
      if outsidebatch[b]:
        x['eachdac',b] += 1

    # clear non-selected primes in batches from P:
    for t in range(targetlen):
      x['maxdac',target[t]] += batchsize[target[t]]-1

  while sum(todo) > 0:
    x['AB'] += 1

    outsidebatch = [todo[b] == 0 for b in range(B)]
    target = [b for b in range(B) if todo[b] > 0]
    targetlen = len(target)
    if targetlen > 3:
      target.reverse()
      # 7 6 5 4 3 2 1 0
      target = target[1:2]+target[3:]+target[2:3]+target[0:1]
      # 6 4 3 2 1 0 5 7

    # generate P0:
    x['elligator'] += 1

    for t in range(targetlen):
      if t == 0:
        clearnonselected() # from P0

      # kernel point:
      for u in range(t+1,targetlen):
        x['maxdac',target[u]] += 1

      success = random.randrange(primes[batchstart[target[t]]]) != 0

      if success:
        if t == targetlen-1:
          push = 0
        elif t == 0:
          push = 1
        else:
          push = 2
        if t == targetlen-2 and targetlen > 2:
          push = 1

        x['isog',push,target[t]] += 1
        todo[target[t]] -= 1

      if t == 0:
        x['elligator'] += 1 # generate P1
        clearnonselected() # from P1

      if t == targetlen-2 and targetlen > 2:
        x['maxdac',target[t]] += 1 # P0
      elif t < targetlen-1:
        x['maxdac',target[t]] += 2 # P0, P1

  assert x['AB'] >= max(batchbound)
  assert x['elligator'] == 2*x['AB']
  assert x['clear2'] == 4*x['AB']
  for b in range(B):
    assert x['isog',0,b]+x['isog',1,b]+x['isog',2,b] == batchbound[b]

  return x

def test():
  sys.setrecursionlimit(10000)

  primes = (3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,587)
  batchsize = (3, 4, 5, 5, 6, 6, 6, 8, 7, 9, 10, 5)
  batchbound = (13, 19, 20, 20, 20, 20, 20, 20, 17, 17, 17, 5)

  total = {}

  trials = 0

  while True:
    x = trial(primes,batchsize,batchbound)

    # print(costs.strstats(x,'%d'))
    for cost in x:
      if cost not in total:
        total[cost] = 0
      total[cost] += x[cost]

    trials += 1
    if trials > 0 and trials&(trials-1) == 0:
      avg = {cost:total[cost]*1.0/trials for cost in total}
      print(costs.strstats(avg,'%s '%trials,'%.6f',primes,batchsize))
      sys.stdout.flush()

if __name__ == '__main__':
  test()
