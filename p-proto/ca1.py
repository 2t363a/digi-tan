S = {}
for s1 in range(-1, 2):
    for s2 in range(-1, 2):
        for s3 in range(-1, 2):
            s = s1 + s2 + s3
            S[s] = True

print (tuple(S.keys()))
