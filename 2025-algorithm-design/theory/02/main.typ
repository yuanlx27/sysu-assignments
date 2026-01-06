#import "@local/sysu-templates:0.3.0" as sysu
#import sysu.zuoye: *

#show: zuoye.with(
  title: "第二章作业",
  subtitle: "算法设计与分析",
  student: (name: "元朗曦", id: "23336294"),
)

= 2.4

给定含有 $n$ 个不同的数的数组 $L = chevron.l x_1, x_2, dots, x_n chevron.r$. 如果 $L$ 中存在 $x_i$ 使得 $x_1 < x_2 < dots < x_(i - 1) < x_i > x_(i + 1) > dots > x_n$, 则称 $L$ 是单峰的, 并称 $x_i$ 是 $L$ 的"峰顶". 假设 $L$ 是单峰的, 设计一个算法找到 $L$ 的峰顶．

*解．*Python 实现如下：

#raw(block: true, lang: "py", read("assets/2-04.py"))

= 2.8

设 $A$ 和 $B$ 都是从小到大已经排好序的 $n$ 个不等的整数构成的数组, 如果把 $A$ 与 $B$ 合并后的数组记作 $C$, 设计一个算法找出 $C$ 的中位数．

*解．*Python 实现如下：

#raw(block: true, lang: "py", read("assets/2-08.py"))

= 2.12

设 $A = {a_1, a_2, dots, a_n}$, $B = {b_1, b_2, dots, b_m}$ 是整数集合, 其中 $m = O(log n)$．设计一个算法找出集合 $C = A inter B$．要求给出伪码描述．

*解．*Python 实现如下：

#raw(block: true, lang: "py", read("assets/2-12.py"))

= 2.24

设 $A$ 是 $n$ 个数构成的数组, 其中出现次数最多的数称为众数, 设计一个算法求 $A$ 的众数, 给出伪码和最坏情况下的时间复杂度．

*解．*Python 实现如下：

#raw(block: true, lang: "py", read("assets/2-24.py"))

时间复杂度为 $O(n log m)$ 也即 $O(n log log n)$．