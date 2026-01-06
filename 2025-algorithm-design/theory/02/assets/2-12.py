def intersection(A, B):
    B.sort()

    for a in A:
        l = 0
        r = len(B) - 1
        while l < r:
            m = (l + r) // 2
            if B[m] < a:
                l = m + 1
            else:
                r = m
        if B[l] == a:
            yield a


A = [1, 2, 3, 4, 5, 6, 7, 8]
B = [2, 3, 5]
print(list(intersection(A, B)))  # Output: [2, 3, 5]