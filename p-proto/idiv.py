import random

def ilog(a):
    i = 0
    while a > 0:
        a >>= 1
        i += 1
    return i

def idiv(a, b):
    ia = ilog(a)
    ib = ilog(b)
    if ia < ib:
        return (0, b)
    n = ia - ib
    q = 0
    p = 1 << n
    while p > 0:
        qq = p + q
        if qq * b < a:
            q = qq
        p >>= 1
    r = a - q * b
    return (q, r)

def __test():
    for i in range(10):
        x = random.randint(1, 1 << 32)
        y = random.randint(1, 1 << 32)
        z = random.randint(0, y - 1)
        if random.random() < 0.333:
            x = 0
        a = x * y + z
        b = y
        q, r = idiv(a, b)
        if x != q:
            print (f"{a:12} / {b:12} = {x:12} + {z:12}")
        if r != z:
            print (f"; {a:12} / {b:12} = {x:12} + {z:12}")
            print (f"{a:12} / {b:12} = {q:12} + {r:12}")




if __name__ == '__main__':
    __test()
