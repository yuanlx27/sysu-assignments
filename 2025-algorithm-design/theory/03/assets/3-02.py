def longest_increasing_subsequence(n, A):
    # dp[i] stores the length of the LIS ending at index i.
    dp = [1 for _ in range(n)]
    # prev[i] stores the previous index of the LIS ending at index i.
    prev = [-1 for _ in range(n)]

    max_idx = 0
    max_len = 1
    for i in range(1, n):
        for j in range(i):
            if A[j] < A[i] and dp[j] + 1 > dp[i]:
                dp[i] = dp[j] + 1
                prev[i] = j
        if dp[i] > max_len:
            max_idx = i
            max_len = dp[i]

    lis = []
    idx = max_idx
    while idx != -1:
        lis.append(A[idx])
        idx = prev[idx]
    lis.reverse()

    return lis

A = [2, 8, 4, -4, 5, 9, 11]
print(longest_increasing_subsequence(len(A), A))