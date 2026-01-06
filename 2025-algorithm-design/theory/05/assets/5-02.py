class MinWeightMachine:
    def __init__(self, max_cost):
        self.max_cost = max_cost
        self.min_weight = float('inf')
        self.best_path = []  # 存储最佳供应商选择

        # 数据结构：data[配件编号-1][供应商编号-1] = (价格, 重量)
        # 对应题目中的表 5.2
        self.data = [
            # 配件 1: 供应商1(10,5), 供应商2(8,6), 供应商3(12,4)
            [(10, 5), (8, 6), (12, 4)],
            # 配件 2: 供应商1(20,8), 供应商2(21,10), 供应商3(30,5)
            [(20, 8), (21, 10), (30, 5)],
            # 配件 3: 供应商1(40,5), 供应商2(42,4), 供应商3(30,10)
            [(40, 5), (42, 4), (30, 10)],
            # 配件 4: 供应商1(30,20), 供应商2(60,10), 供应商3(45,15)
            [(30, 20), (60, 10), (45, 15)]
        ]

        self.n = len(self.data)  # 配件数量

    def backtrack(self, component_idx, current_cost, current_weight, current_path):
        """
        component_idx: 当前正在考虑的配件编号 (0 ~ 3)
        current_cost: 当前累计价格
        current_weight: 当前累计重量
        current_path: 当前选择的供应商列表
        """

        # 剪枝条件 1：如果当前累计价格已经超过最大预算，停止搜索
        if current_cost > self.max_cost:
            return

        # 剪枝条件 2：如果当前累计重量已经超过（或等于）目前已知的最小重量，停止搜索
        # (这步是分支限界的核心，可以大幅减少计算量)
        if current_weight >= self.min_weight:
            return

        # 基本情况：如果已经选完了所有配件 (0, 1, 2, 3 都选完了)
        if component_idx == self.n:
            self.min_weight = current_weight
            self.best_path = list(current_path)
            return

        # 递归步骤：遍历当前配件的 3 个供应商
        for supplier_idx in range(3):
            cost, weight = self.data[component_idx][supplier_idx]

            # 选择该供应商
            current_path.append(supplier_idx + 1)  # 记录供应商编号 (1-3)

            # 进入下一层递归
            self.backtrack(
                component_idx + 1,
                current_cost + cost,
                current_weight + weight,
                current_path
            )

            # 回溯：撤销选择，尝试下一个供应商
            current_path.pop()

    def solve(self):
        self.backtrack(0, 0, 0, [])

        if self.min_weight == float('inf'):
            print("没有找到满足价格限制的方案。")
        else:
            print("-" * 30)
            print(f"最优方案结果 (价格上限: {self.max_cost}):")
            print(f"最小总重量: {self.min_weight}")
            print("-" * 30)
            total_check_cost = 0
            for i, supplier in enumerate(self.best_path):
                # 获取具体数据用于展示
                c, w = self.data[i][supplier - 1]
                total_check_cost += c
                print(f"配件 {i + 1} -> 选择供应商 {supplier} (价格: {c}, 重量: {w})")
            print("-" * 30)
            print(f"总价格: {total_check_cost}")


if __name__ == "__main__":
    # 限制总价值不超过 120
    solver = MinWeightMachine(max_cost=120)
    solver.solve()
