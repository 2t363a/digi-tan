import math

x0 = math.pi

x = x0
I = int(math.floor(x))
x -= I

R = []

A = [1, I]
B = [0, 1]
eps = 1.0
while eps > 1e-15:
    x = 1.0 / x
    ip = int(math.floor(x))
    x -= ip
    R += [ip]
    Anext = ip * A[-1] + A[-2]
    Bnext = ip * B[-1] + B[-2]
    A[-2], A[-1] = A[-1], Anext
    B[-2], B[-1] = B[-1], Bnext
    app = Anext / Bnext
    eps  = math.fabs(app - x0) / x0
    print (ip, eps, Anext, Bnext)

print (I, R)

