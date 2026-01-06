def solve_assignment_recursive(cost_matrix):
    n = len(cost_matrix)
    best_assignment = None
    best_cost = float('inf')

    def backtrack(person, used_jobs, current_cost, assignment):
        nonlocal best_cost, best_assignment

        if person == n:
            if current_cost < best_cost:
                best_cost = current_cost
                best_assignment = assignment[:]
            return

        for job in range(n):
            if job not in used_jobs:
                if current_cost + cost_matrix[person][job] < best_cost:
                    assignment[person] = job
                    used_jobs.add(job)
                    backtrack(person + 1, used_jobs, current_cost + cost_matrix[person][job], assignment)
                    used_jobs.remove(job)

    backtrack(0, set(), 0, [0] * n)
    return best_assignment, best_cost
