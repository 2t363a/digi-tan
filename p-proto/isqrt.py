def isqrt(x):
    p = 1
    n = 0
    while p * p <= x:
        p *= 2
        n += 1
    p >>= 1
    if p == x: return p
    t = p
    p >>= 1
    while p > 0:
        tt = t + p
        if tt * tt < x:
            t += p
        p >>= 1
    return t

TESTCASES = (0, 1, 5, 127, 256, 4370234823907850238508)

for t in TESTCASES:
    print (f"| {t: 48}  | {isqrt(t):25} |")
