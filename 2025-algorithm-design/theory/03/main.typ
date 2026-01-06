#import "@local/sysu-templates:0.3.0" as sysu
#import sysu.zuoye: *

#show: zuoye.with(
  title: "第三章作业",
  subtitle: "算法设计与分析",
  student: (name: "元朗曦", id: "23336294"),
)

= 3.2

设 $A = chevron.l x_1, x_2, dots, x_n chevron.r$ 是 $n$ 个不等的整数构成的序列, $A$ 的一个单调递增子序列是序列 $chevron.l x_(i_1), x_(i_2), dots, x_(i_k) chevron.r$ 使得 $i_1 < i_2 < dots < i_k$, 且 $x_(i_1) < x_(i_2) < dots < x_(i_k)$．子序列 $chevron.l x_(i_1), x_(i_2), dots, x_(i_k) chevron.r$ 的长度是含有的整数个数 $k$．例如 $A = chevron.l 1, 5, 3, 8, 10, 6, 4, 9 chevron.r$, 它的长为 4 的递增子序列是：$chevron.l 1, 5, 8, 10 chevron.r$, $chevron.l 1, 5, 8, 9 chevron.r$, $dots$．设计一个算法求 $A$ 的一个最长的单调递增子序列, 分析算法的时间复杂度．设算法的输入实例是 $A = chevron.l 2, 8, 4, -4, 5, 9, 11 chevron.r$, 给出算法的计算过程和最后的解．

*解．*经典题。可用动态规划算法在 $O(n^2)$ 复杂度内求解，或用二分查找算法在 $O(n log n)$ 内求解．

Python 实现如下：

#raw(block: true, lang: "py", read("assets/3-02.py"))

= 3.10

把 0-1 背包问题加以推广．设有 $n$ 种物品，第 $i$ 种物品的价值是 $v_i$，重量是 $w_i$，体积是 $c_i$，且装入背包的重量限制是 $W$，体积是 $V$．问如何选择装入背包的物品使得其总重不超过 $W$，总体积不超过 $V$ 且价值达到最大？设计一个动态规划算法求解这个问题，说明算法的时间复杂度．

*解．*与普通的 0-1 背包类似，在动态规划算法的状态中多加一维来处理第二个限制．

Python 实现如下：

#raw(block: true, lang: "py", read("assets/3-10.py"))

时间复杂度为 $O(n W V)$．

= 3.15

某机器每天接受大量加工任务，第 $i$ 天需要加工的任务数是 $x_i$．随着机器连续运行时间的增加，处理能力越来越低，需要花 $1$ 天时间对机器进行检修，以提高处理能力．检修当天必须停工，重启后的第 $i$ 天能够加工的任务数是 $s_i$，且满足

$ s_1 > s_2 > dots > s_n > 0 $

我们的问题是：给定 $x_1, x_2, dots, x_n$ 和 $s_1, s_2, dots, s_n$，如何安排机器的检修时间，以使得在 $n$ 天内加工的任务数达到最大？设计一个算法求解该问题．

*解．*设 $"dp"(i, j)$ 表示在前 $i$ 天内，距上次检修经过了 $j$ 天（检修当天对应 $j = 0$ 的情况），能加工的最大任务数．则有状态转移方程：

$
cases(
  "dp"(i, 0) = max_(0 <= k <= i - 1) "dp"(i - 1, k),
  "dp"(i, j) = "dp"(i - 1, j - 1) + min{x_i, s_j} quad "for" 0 < j <= i,
)
$

Python 实现如下：

#raw(block: true, lang: "py", read("assets/3-15.py"))