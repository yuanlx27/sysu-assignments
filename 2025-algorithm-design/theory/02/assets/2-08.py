def find_median(A, B):
    n = len(A)

    l = 0
    r = n
    while l <= r:
        m = (l + r) // 2

        al = A[m - 1] if m > 0 else float('-inf')
        ar = A[m] if m < n else float('inf')
        bl = B[n - m - 1] if m < n else float('-inf')
        br = B[n - m] if m > 0 else float('inf')

        if al <= br and bl <= ar:
            return (max(al, bl) + min(ar, br)) / 2

        if al > br:
            r = m - 1
        else:
            l = m + 1


A = [1, 2, 3, 4]
B = [5, 6, 7, 8]
print(find_median(A, B))  # Output: 4.5