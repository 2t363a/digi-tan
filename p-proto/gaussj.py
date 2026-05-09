from copy import deepcopy as dup
import math
import sys

def ipower(x, n):
    if n < 0: return 1 / ipower(x, -n)
    if n == 0: return 1
    if n == 1: return x
    if n == 2: return x * x
    if n == 3: return x * x * x
    t = ipower(x, n // 2)
    t *= t
    if (n % 2) > 0:
        t *= x
    return t

def read_file(path):
    X = []
    Y1 = []
    Y2 = []
    with open(path, mode='rt') as fin:
        for l in fin:
            l = l.strip()
            l = l.split(',')
            if len(l) != 4: continue
            if (l[0] != 'data'): continue
            X += [int(l[1])]
            Y1 += [int(l[2])]
            Y2 += [int(l[3])]
    return X, Y1, Y2

def gauss_forward(n, M, V):
    reorder = [i for i in range(n)]
    i = 0
    while i < n:
        imax = i
        for ii in range(n):
            if math.fabs(M[reorder[ii]][i]) > math.fabs(M[reorder[imax]][i]):
                imax = ii
        reorder[imax], reorder[i] = reorder[i], reorder[imax]
        row_i = M[reorder[i]]
        lc = row_i[i]
        for k in range(i + 1, n):
            row_k = M[reorder[k]]
            q = lcm(lc, row_k[i])
            k_rowi = q // lc
            k_rowk = q // row_k[i]
            s_rowi = [x * k_rowi for x in row_i]
            s_rowk = [x * k_rowk for x in row_k]
            nr2 = [s_rowk[j] - s_rowi[j] for j in range(0, n)]
            M[reorder[k]] = nr2
            V[reorder[k]] = k_rowk * V[reorder[k]] - k_rowi * V[reorder[i]]
        i += 1
    return reorder

def jordan(n, M, R, V):
    i = n - 1
    X = [0] * n
    while i >= 0:
        row = M[R[i]]
        b = V[R[i]]
        for j in range(n - 1, i, -1):
            b -= X[j] * row[j]
        X[i] = b / row[i]
        i -= 1
    return X

def gauss_elim(M, V):
    M = dup(M)
    V = dup(V)
    n = len(M)
    reorder = gauss_forward(n, M, V)
    X = jordan(n, M, reorder, V)
    return X

def lsq(deg, X, Y):
    nc = deg + 1
    SX = []
    SY = []
    for i in range(2 * deg + 1):
        sx = sum([ipower(x, i) for x in X])
        SX += [sx]
    for i in range(deg + 1):
        sy = 0
        for j, x in enumerate(X):
            sy += Y[j] * ipower(x, i)
        SY += [sy]
    M = []
    for i in range(nc):
        ROW = []
        for j in range(nc):
            ROW += [SX[i + j]]
        M += [ROW]
    gauss_elim(M, SY)

def test():
    M = [[2, 3, 5], [10, 8, 15], [6, 20, 7]]
    V = [10, 20, 30]
    X = gauss_elim(M, V)
    dr2 = 0.0
    for row, v in zip(M, V):
        dr = sum([a * x for a, x in zip(row, X)]) - v
        dr2 += dr * dr
    # нужна более устойчивая мера типа ~ ||V - v|| * ||A^{-1}||
    if dr2 > 1e-12:
        print ('dr2 > 1e-12')
        print ('FAIL')
        sys.exit(1)
    print (M, V, X)
    print ('PASS')

def main():
    X, Y1, Y2 = read_file('./data')
    lsq(1, X, Y1)

test()

