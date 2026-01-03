def longest_substring(s):
    n = len(s)

    # dp[i][j] denotes the length of the longest
    # non-overlapping common prefix of s[i..n-1] and s[j..n-1].
    dp = [[0] * (n + 1)] * (n + 1)

    for i in reversed(range(0, n)):
        for j in reversed(range(i, n)):
            if s[i] == s[j]:
                dp[i][j] = min(dp[i + 1][j + 1] + 1, j - i)

    p, m = 0, 0
    for i in range(0, n):
        for j in range(i, n):
            if dp[i][j] > m:
                p, m = i, dp[i][j]

    return s[p:p + m]


if __name__ == "__main__":
    print(longest_substring("geeksforgeeks"))  # Output: geeks
    print(longest_substring("aabaabaaba"))  # Output: aaba
