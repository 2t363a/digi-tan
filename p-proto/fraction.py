class Fraction:
    def __init__(self, _a, _b = 1):
        self._nom = _a
        self._denom = _b

    def __gcd2(self, a, b):
        if a == 0: return b
        if b == 0: return a
        if a < b: return self.__gcd2(a, b % a)
        return self.__gcd2(b, a % b)

    def __gcd(self, a, b):
        if a < 0 and b < 0: return self.__gcd2(-a, -b)
        elif a < 0: return -self.__gcd2(-a, b)
        elif b < 0: return -self.__gcd2(a, -b)
        return self.__gcd2(b, a % b)

    def __lcm(self, a, b):
        return (a * b) // gcd(a, b)

    def __normalize(self):
        x = self.__gcd(self._nom, self._denom)
        self._nom /= x
        self._denom /= x
        return self

    def add(self, f):
        nom1 = f._nom * self._denom
        nom2 = self._nom * f._denom
        denom = self._denom * f._denom
        nom = nom1 + nom2
        r = Fraction(nom, denom)
        return r.__normalize()

    def sub(self, f):
        nom1 = f._nom * self._denom
        nom2 = self._nom * f._denom
        denom = self._denom * f._denom
        nom = nom1 - nom2
        r = Fraction(nom, denom)
        return r.__normalize()


def lcm(a, b):
    return (a * b) // gcd(a, b)

def gcd(a, b):
    if a == 0: return b
    if b == 0: return a
    if a < b: return gcd(a, b % a)
    return gcd(b, a % b)


