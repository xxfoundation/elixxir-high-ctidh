#!/usr/bin/env python3

from memoized import memoized

def twovaluation(x):
  if x == 0: return 0
  y = x
  result = 0
  while not y&1:
    y //= 2
    result -= 1
  return result

# same as in https://cr.yp.to/papers.html#efd
# except that here we exclude negative r
def chain(m,n):
  assert m >= 3
  assert m&1
  assert n >= 0

  if n == 2 or ((n&1) and n <= m): 
    return [1,2]+list(range(3,m+1,2))

  if n == m+2:
    return chain(m,m)+[n]

  if n%6 == 1 and m+4 <= n and n <= 3*m-2:
    return chain(m,m)+[(2*n+4)//3,n]

  if n%6 == 3 and m+4 <= n and n <= 3*m:
    return chain(m,m)+[(2*n)//3,n]

  if n%6 == 5 and m+4 <= n and n <= 3*m-4:
    return chain(m,m)+[(2*n-4)//3,n]

  if n%4 == 0 and 4 <= n and n <= 2*m-2:
    return chain(m,m)+[n]

  if n&1:
    L = [(twovaluation(n-r),r) for r in range(1,m+1,2)]
    L.sort()
    _,r = L[0]
    return chain(m,n-r)+[n]
  return chain(m,n//2)+[n]

def cost2(C):
  result = [0,0]
  for n in C:
    if n == 1:
      continue
    if n&1 == 0 and n//2 in C:
      result[1] += 1
    else:
      assert any(n-m in C for m in C)
      result[0] += 1
  return tuple(result)

def cost(C):
  result = cost2(C)
  return 10*result[0]+8*result[1]

@memoized
def chain2(n):
  m = 3
  bestm = 3
  C = chain(m,n)
  while True:
    m += 2
    if 2*m > 3*bestm+10:
      return C
    C2 = chain(m,n)
    if cost(C2) < cost(C):
      bestm = m
      C = C2

def code(C):
  result = ''

  insn = []
  uses = {}

  for n in C:
    uses[n] = 0
    if n == 1:
      insn += [(1,'init',())]
      continue
    if n&1 == 0 and n//2 in C:
      rep = 1
      m = n//2
      while m&1 == 0 and m//2 in C:
        rep += 1
        m //= 2
      insn += [(n,'sq_rep',(m,-rep))]
      uses[m] += 1
      continue
    ok = False
    for m in C:
      if n-m in C:
        insn += [(n,'mul',(m,n-m))]
        uses[m] += 1
        uses[n-m] += 1
        ok = True
        break
    assert ok

  uses[max(C)] = 1

  decl = set() # registers declared
  regs = set() # currently used registers
  m2reg = {} # mapping m to register containing m

  for j in range(len(insn)):
    n,op,inputs = insn[j]

    clearregs = []

    for m in inputs:
      if m < 0: continue
      assert uses[m] >= 1
      uses[m] -= 1
      if uses[m] == 0:
        clearregs += [m]

    if uses[n] == 0: continue

    if len(clearregs) > 0:
      nreg = min(m2reg[m] for m in clearregs)
    else:
      nreg = 0
      while nreg in regs:
        nreg += 1

    if nreg not in decl:
      result += '  fp r%d;\n' % nreg
      decl.add(nreg)

    if op == 'init':
      assert len(inputs) == 0
      result += '  r%d = *x; // %d\n' % (nreg,n)

    if op == 'sq_rep':
      assert len(inputs) == 2
      m,rep = inputs
      rep = -rep
      if nreg != m2reg[m]:
        result += '  fp_sq2(&r%d,&r%d);\n' % (nreg,m2reg[m])
        rep -= 1
      if rep > 0:
        if rep > 1:
          result += '  fp_sq1_rep(&r%d,%d);\n' % (nreg,rep)
        else:
          result += '  fp_sq1(&r%d);\n' % (nreg)

    if op == 'mul':
      assert len(inputs) == 2
      m1,m2 = inputs
      if nreg == m2reg[m1]:
        result += '  fp_mul2(&r%d,&r%d); // %d\n' % (nreg,m2reg[m2],n)
      elif nreg == m2reg[m2]:
        result += '  fp_mul2(&r%d,&r%d); // %d\n' % (nreg,m2reg[m1],n)
      else:
        result += '  fp_mul3(&r%d,&r%d,&r%d); // %d\n' % (nreg,m2reg[m1],m2reg[m2],n)

    for m in clearregs:
      regs.remove(m2reg[m])
      m2reg.pop(m)

    m2reg[n] = nreg
    regs.add(nreg)

  result += '  *x = r%d;\n' % nreg
  return result
