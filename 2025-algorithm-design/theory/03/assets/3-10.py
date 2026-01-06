def knapsack(n, v, w, c, W, V):
    # dp[i][w][v] stores the maximum value using the first i items
    # with weight limit w and volume limit v.
    dp = [[[0] * (V + 1) for _ in range(W + 1)] for _ in range(n + 1)]

    for i in range(1, n + 1):
        vi = v[i - 1]
        wi = w[i - 1]
        ci = c[i - 1]
        for w in range(W + 1):
            for v in range(V + 1):
                dp[i][w][v] = dp[i - 1][w][v]
                if w >= wi and v >= vi:
                    dp[i][w][v] = max(dp[i][w][v], dp[i - 1][w - wi][v - vi] + ci)

    return dp[n][W][V]