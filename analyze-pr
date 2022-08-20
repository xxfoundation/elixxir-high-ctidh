#!/usr/bin/env python3

import sys

pp1ready = False
batchboundready = False

data = {}
datanum = {}
keys = set()

for line in sys.stdin:
  line = line.split()
  if len(line) <= 4: continue
  if line[2] != 'action': continue
  if line[3] == 'pp1':
    pp1 = [float(x) for x in line[4:]]
    pp1ready = True
    continue
  if line[3] == 'batchbound':
    batchbound = [int(x) for x in line[4:]]
    batchboundready = True
    continue
  if line[3] == 'stattried':
    assert pp1ready
    assert batchboundready
    stats = line[4:]
    assert len(stats) == len(pp1)
    keys.add(int(line[1]))
    for k in int(line[1]),'total':
      if k not in data:
        data[k] = [0]*len(pp1)
        datanum[k] = 0
      datanum[k] += 1
      for i in range(len(pp1)):
        data[k][i] += int(stats[i])

for k in sorted(keys)+['total']:
  output = '%s'%k
  for i in range(len(pp1)):
    if batchbound[i] == 0:
      assert data[k][i] == 0
      output += ' 1'
    else:
      output += ' %.6f' % (data[k][i]/(datanum[k]*batchbound[i]*pp1[i]))
  print(output)
