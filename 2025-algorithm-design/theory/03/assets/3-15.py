def machine_maintenance(x, s, n):
    # dp[i][j] stores the maximum tasks processed in first i days,
    # with j days since last maintenance.
    dp = [[0] * (n + 1) for _ in range(n + 1)]
    
    for i in range(1, n + 1):
        for j in range(i):
            dp[i][0] = max(dp[i][0], dp[i - 1][j])
        for j in range(1, i + 1):
            dp[i][j] = dp[i - 1][j - 1] + min(x[i - 1], s[j - 1])
    
    return max(dp[n])

x = [10, 20, 30, 40, 50]
s = [50, 40, 30, 20, 10]
print(machine_maintenance(x, s, len(x)))