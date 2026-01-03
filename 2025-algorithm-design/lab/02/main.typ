#import "@local/sysu-templates:0.2.0": report

#show: report.with(
  title: "第三章 动态规划",
  subtitle: "算法设计与分析上机作业二",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 问题描述

给定字符串，找出其中最长的重复出现并且非重叠的子字符串。

例子：

输入：`str = "geeksforgeeks"`

输出：`geeks`

输入：`str = "aabaabaaba"`

输出：`aaba`

= 实现思路

令 $"dp"(i, j)$ 表示以 $i$ 和 $j$ 开头的后缀的最长公共前缀的长度，也即以 $i$ 和 $j$ 开头的最长重复子串的长度。我们很容易写出状态转移方程：

$
  "dp"(i, j) = cases(
    "dp"(i + 1, j + 1) + 1 wide & "if" "str"_i = "str"_j,
    0 wide & "otherwise",
  )
$

为了保证不重叠，我们可以在状态转移时与 $|i - j|$ 取最小值，即：

$
  "dp"(i, j) = cases(
    min{"dp"(i + 1, j + 1) + 1, |i - j|} wide & "if" "str"_i = "str"_j,
    0 wide & "otherwise",
  )
$

由于对称性，我们只需要考虑 $i <= j$ 的情况。

完整代码如下：

#raw(block: true, lang: "python", read("assets/main.py"))

= 运行结果

在终端中运行 ```sh python assets/main.py```，结果如下：

#figure(
  caption: [运行结果],
  image("assets/images/20260103-134745.png"),
)
