radical(x) = {
    my(Primes = factor(x)[,1]~);
    my(L = #Primes);
    my(R = 1);
    for(i=1,L, R = R * Primes[i]);
    return (R)
}

quality(x) = {
    my(Primes = factor(x)[,1]~);
    my(L = #Primes);
    my(R = 1);
    for(i=1,L, R = R * Primes[i]);
    return (R / x)
}
