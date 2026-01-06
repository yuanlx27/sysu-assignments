def find_modes(A):
    if not A:
        return None

    frequency = {}
    for a in A:
        frequency[a] = frequency.get(a, 0) + 1

    max_count = max(frequency.values())
    modes = [key for key, count in frequency.items() if count == max_count]

    return modes


A = [1, 2, 2, 3, 3, 3, 2]
print(find_modes(A))