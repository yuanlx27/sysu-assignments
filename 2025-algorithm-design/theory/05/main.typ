#import "@local/sysu-templates:0.3.0" as sysu
#import sysu.zuoye: *

#show: zuoye.with(
  title: "第五章作业",
  subtitle: "算法设计与分析（理论）",
  student: (name: "元朗曦", id: "23336294"),
)

= 5.2

最小重量机器设计问题．某设备需要 4 种配件，每种 1 件．有三个供应商提供这些配件，@tab-1 给出了相关配件的价格和每种配件的重量．试从中选择这 4 种配件，使得总价值不超过 120，且总重量最轻．

#figure(
  caption: [原书表 5.2],
  table(
    align: horizon,
    columns: 7,
    table.header(
      table.cell(rowspan: 2)[*配件编号*],
      table.cell(colspan: 2)[*供应商 1*],
      table.cell(colspan: 2)[*供应商 2*],
      table.cell(colspan: 2)[*供应商 3*],
      [*价格*], [*重量*], [*价格*], [*重量*], [*价格*], [*重量*],
    ),
    [1], [10], [5], [8], [6], [12], [4],
    [2], [20], [8], [21], [10], [30], [5],
    [3], [40], [5], [42], [4], [30], [10],
    [4], [30], [20], [60], [10], [45], [15],
  ),
) <tab-1>

#solution[
  所有可能的组合共有 $3^4 = 81$ 种，可穷举求解．代码如下：

  #raw(block: true, lang: "python", read("assets/5-02.py"))
]

= 5.7

分派问题．给 $n$ 个人分配 $n$ 件工作，给第 $i$ 个人分配第 $j$ 件工作的成本是 $C(i, j)$．试求成本最小的工作分配方案．

#solution[
  可以枚举排列 $p$，其中 $p(i)$ 表示第 $i$ 个人分配到的工作编号．

  此时总成本为 $sum_(i = 1)^n C(i, p(i))$．代码如下：

  #raw(block: true, lang: "python", read("assets/5-07.py"))

  也可以视作二分图最小权匹配，或最小费用最大流问题，用相关算法求解．
]

= 5.9

设有 $n$ 项任务由 $k$ 个可并行操作的机器完成，完成任务 $i$ 所需要的时间是 $t_i$，求一个最佳任务分配方案，使得完成时间（即从时刻 0 开始计时到最后一台机器停止的时间）达到最短．

#solution[
  可以枚举每个任务分配到的机器编号，计算每台机器的总任务时间，并取所有方案的最小值．代码如下：

  #raw(block: true, lang: "python", read("assets/5-09.py"))
]
