class BestSchedule:
    def __init__(self, k, times):
        """
        k: 机器数量
        times: 任务时间列表 [t1, t2, ...]
        """
        self.k = k
        self.raw_times = times
        # 优化策略1：将任务按时间从大到小排序 (LPT策略)
        # 这有助于更快地找到较优解，从而进行更有效的剪枝
        self.sorted_tasks = sorted(times, reverse=True)
        self.n = len(times)

        self.best_makespan = float('inf')
        self.best_assignment = [[] for _ in range(k)]

        # 当前各机器的累计时间
        self.current_loads = [0] * k
        # 当前的分配方案（记录的是任务的时间）
        self.current_assignment = [[] for _ in range(k)]

    def backtrack(self, task_idx):
        # 基本情况：所有任务都已分配
        if task_idx == self.n:
            current_max_time = max(self.current_loads)
            if current_max_time < self.best_makespan:
                self.best_makespan = current_max_time
                # 深拷贝当前的分配方案
                self.best_assignment = [list(machine) for machine in self.current_assignment]
            return

        # 剪枝策略2：如果当前任何一台机器的时间已经超过了已知的最优解
        # 那么这个分支不可能产生更好的结果，直接回溯
        if max(self.current_loads) >= self.best_makespan:
            return

        task_time = self.sorted_tasks[task_idx]

        for i in range(self.k):
            # 尝试将当前任务分配给机器 i

            # 小优化：如果加上当前任务会超过已知最优解，跳过该机器
            if self.current_loads[i] + task_time >= self.best_makespan:
                continue

            self.current_loads[i] += task_time
            self.current_assignment[i].append(task_time)

            # 递归处理下一个任务
            self.backtrack(task_idx + 1)

            # 回溯：撤销选择
            self.current_loads[i] -= task_time
            self.current_assignment[i].pop()

            # 关键剪枝优化：
            # 如果当前机器是空的（load为0），那么将任务放入这台空机器
            # 和放入后面的其他空机器是等效的（对称性）。
            # 所以如果从一台空机器回溯回来，就不需要尝试后面所有的机器了。
            if self.current_loads[i] == 0:
                break

    def solve(self):
        # 预估下界：理论上最快的时间是所有任务总和除以机器数
        # 或者是最大的那个单任务时间（因为任务不可拆分）
        # lower_bound = max(max(self.sorted_tasks), sum(self.sorted_tasks) / self.k)
        # print(f"理论下界: {lower_bound}")

        self.backtrack(0)

        print("-" * 30)
        print(f"任务总数: {self.n}, 机器数量: {self.k}")
        print(f"任务时间: {self.raw_times}")
        print("-" * 30)
        print(f"最短完成时间 (Makespan): {self.best_makespan}")
        print("-" * 30)
        for i, machine_tasks in enumerate(self.best_assignment):
            print(f"机器 {i + 1}: {machine_tasks} (总计: {sum(machine_tasks)})")
        print("-" * 30)


if __name__ == "__main__":
    # 示例数据 1
    # 7个任务，3台机器
    k_machines = 3
    task_times = [2, 14, 4, 16, 6, 5, 3]

    solver = BestSchedule(k_machines, task_times)
    solver.solve()

    print("\n" + "=" * 40 + "\n")

    # 示例数据 2 (稍微复杂一点)
    k_machines_2 = 4
    task_times_2 = [10, 8, 6, 12, 20, 5, 4, 15]

    solver2 = BestSchedule(k_machines_2, task_times_2)
    solver2.solve()
