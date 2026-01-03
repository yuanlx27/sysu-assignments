#import "@local/sysu-templates:0.2.0": report

#show: report.with(
  title: "第二章 分治策略",
  subtitle: "算法设计与分析上机作业一",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 问题描述

对于给定的 $N$ 本书和 $M$ 个学生，每本书的页数已经按升序排列。我们的任务是分配这些书，使得每个学生阅读页数的最大值最小，并且每个学生阅读的书需要在原排列中连续。输出这个最小的最大值。

= 实现思路

题目要求“最小化最大值”。由于答案具有单调性，我们可以对答案进行二分查找。对于每一个候选值 $x$，我们考虑贪心地给每个学生分配尽可能多但不超过 $x$ 的页数，若最后所有书分配给了不超过 $M$ 个学生，则 $x$ 是可行的，可以继续搜索更小的值，否则只能搜索更大的值。时间复杂度为 $O(N log V)$，其中 $V$ 为 $N$ 本书的总页数。

完整代码如下：

#raw(block: true, lang: "python", read("assets/main.py"))

= 运行结果

在终端运行 ```sh python assets/main.py``` 结果如下：

#figure(
  caption: [运行结果],
  image("assets/images/20260103-135659.png"),
)
