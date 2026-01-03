def allocate_books(pages, m):
    def check(lim):
        num, tot = 0, 0
        for page in pages:
            if tot + page > lim:
                num += 1
                tot = 0
            tot += page
        if tot > 0:
            num += 1
        return num <= m

    lb, rb = max(pages), sum(pages)
    while lb < rb:
        mid = (lb + rb) // 2
        if check(mid):
            rb = mid
        else:
            lb = mid + 1
    return lb


if __name__ == "__main__":
    pages = [12, 34, 67, 90]
    m = 2
    print(allocate_books(pages, m))  # Output: 113
