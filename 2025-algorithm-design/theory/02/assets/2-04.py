def find_peak(x):
    l = 0
    r = len(x) - 1
    while l < r:
        m = (l + r) // 2
        if x[m] < x[m + 1]:
            l = m + 1
        else:
            r = m
    return x[l]

x = [1, 3, 5, 7, 6, 4, 2]
print(find_peak(x))  # Output: 7