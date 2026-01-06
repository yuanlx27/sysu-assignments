#import "@local/sysu-templates:0.3.0" as sysu
#import sysu.zuoye: *

#show: zuoye.with(
  title: "第二章作业",
  subtitle: "算法设计与分析",
  student: (name: "元朗曦", id: "23336294"),
)

= 2.4

给定含有 $n$ 个不同的数的数组 $L = chevron.l x_1, x_2, dots, x_n chevron.r$. 如果 $L$ 中存在 $x_i$ 使得 $x_1 < x_2 < dots < x_(i - 1) < x_i > x_(i + 1) > dots > x_n$, 则称 $L$ 是单峰的, 并称 $x_i$ 是 $L$ 的"峰顶". 假设 $L$ 是单峰的, 设计一个算法找到 $L$ 的峰顶．

#solution[
  考虑二分查找．对于某个位置 $k$，比较 $x_k$ 与 $x_(k + 1)$ 的大小：

  - 如果 $x_k < x_(k + 1)$，则峰顶在右侧；

  - 如果 $x_k > x_(k + 1)$，则峰顶在左侧，或者 $k$ 就是峰顶．

  Python 实现如下：

  #raw(block: true, lang: "py", read("assets/2-04.py"))
]

= 2.8

设 $A$ 和 $B$ 都是从小到大已经排好序的 $n$ 个不等的整数构成的数组, 如果把 $A$ 与 $B$ 合并后的数组记作 $C$, 设计一个算法找出 $C$ 的中位数．

#solution[
  设在 $C$ 的前 $n$ 个元素中有 $k$ 个元素来自 $A$，则有 $n - k$ 个元素来自 $B$．应有

  $
    A_k <= B_(n - k + 1), quad B_(n - k) <= A_(k + 1).
  $

  由于 $A$ 和 $B$ 都是有序的，因此可以使用二分查找来确定 $k$ 的值．

  Python 实现如下：

  #raw(block: true, lang: "py", read("assets/2-08.py"))
]

= 2.12

设 $A = {a_1, a_2, dots, a_n}$, $B = {b_1, b_2, dots, b_m}$ 是整数集合, 其中 $m = O(log n)$．设计一个算法找出集合 $C = A inter B$．要求给出伪码描述．

#solution[
  容易想到对 $A$ 排序后对 $B$ 中的每个元素使用二分查找来判断其是否在 $A$ 中出现，或使用双指针法同时遍历 $A$ 和 $B$ 来找出交集．时间的瓶颈在于对 $A$ 的排序, 复杂度为 $O(n log n)$．

  然而，由于 $m = O(log n)$ 很小, 我们可以对 $B$ 排序，之后对 $A$ 中的每个元素使用二分查找来判断其是否在 $B$ 中出现．这样总的时间复杂度为 $O(n log m)$ 也即 $O(n log log n)$．

  Python 实现如下：

  #raw(block: true, lang: "py", read("assets/2-12.py"))
]

= 2.24

设 $A$ 是 $n$ 个数构成的数组, 其中出现次数最多的数称为众数, 设计一个算法求 $A$ 的众数, 给出伪码和最坏情况下的时间复杂度．

#solution[
  使用哈希表统计每个数出现的次数, 然后找出出现次数最多的数．

  Python 实现如下：

  #raw(block: true, lang: "py", read("assets/2-24.py"))
]

时间复杂度为 $O(n log m)$ 也即 $O(n log log n)$．