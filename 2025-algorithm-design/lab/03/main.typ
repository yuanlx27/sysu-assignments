#import "@local/sysu-templates:0.2.0": report

#show: report.with(
  title: "第七章 网络流算法",
  subtitle: "算法设计与分析上机作业三",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 问题描述

应用最大流最小割集思想对图像进行分割。例如：

#figure(
  caption: [图像分割示例],
  image("assets/images/sample.png"),
)

= 实现思路

== 图的构建

我们将图像映射为一个无向图 $G = (V, E)$：

- 节点：

  - 每个像素点对应图中的一个普通节点。

  - 增加两个特殊终端节点：源点 $S$ 代表前景，汇点 $T$ 代表背景。

- 边：

  - N-links（Neighbor links）：连接相邻像素的边（采用四相邻连接）。这些边代表了像素之间的连续性或平滑性约束。

  - T-links（Terminal links）：连接像素节点与终端节点（$S$ 或 $T$）的边。这些边代表了像素属于前景或背景的概率（数据项约束）。

== 边的容量定义

+ N-links：

  相邻像素 $u, v$ 之间的边容量 $C_(u, v)$ 反映了它们属于同一区域的可能性。如果两个像素颜色相近，容量应较大（惩罚割断）；如果颜色差异大，容量应较小（容易割断，形成边界）。

  公式如下：

  $ C(u, v) = lambda_"smooth" dot exp(-beta |I_u - I_v|^2) $

其中：

  - $I_u, I_v$ 是像素的颜色强度。

  - $beta$ 是一个缩放参数，通常取值为 $1 / (2 dot chevron.l |I_u - I_v|^2 chevron.r)$（所有相邻像素颜色差平方的均值的倒数），以确保指数项具有适当的动态范围。

+ T-links：

  像素 $u$ 与 $S$ 或 $T$ 连接的边容量反映了该像素属于前景或背景的先验概率。

  + *硬约束*：

    - *前景种子（FG Seeds）*: 图像中心区域（20% 范围）。
  
      - $C(S, u) = oo$ (实际上取一个极大值 $K$)

      - $C(u, T) = 0$

    - *背景种子（BG Seeds）*: 图像边缘区域（外围 2% 范围）。

      - $C(S, u) = 0$

      - $C(u, T) = oo$

  + *软约束*：

    对于非种子区域的像素，我们基于颜色模型计算其属于前景或背景的概率。

    - 计算前景种子和背景种子的平均颜色 $mu_"fg", mu_"bg"$。

    - 计算像素颜色与均值的欧氏距离 $D_"fg" = |I_u - mu_"fg"|^2, D_"bg" = |I_u - mu_"bg"|^2$。

    - 容量定义：
  
      - $C(S, u) = lambda_"data" dot (D_"bg") / {D_"fg" + D_"bg"}$（如果像素像前景，$D_"fg"$ 小，$D_"bg"$ 大，则连接 $S$ 的容量大，不易被割断）

      - $C(u, T) = lambda_"data" dot (D_"fg") / {D_"fg" + D_"bg"}$（如果像素像背景，连接 $T$ 的容量大）

== 最大流最小割求解

根据*最大流最小割定理（Max-Flow Min-Cut Theorem）*，网络中的最大流值等于最小割的容量。

- *算法*：使用 *Edmonds-Karp 算法* 计算最大流。

  - 该算法通过 BFS (广度优先搜索) 在残留网络中不断寻找增广路径 (Augmenting Paths)。

  - 沿增广路径增加流量，直到无法找到新的路径为止。

- *最小割提取*：

  - 最大流计算完成后，在残留网络中从源点 $S$ 开始进行 BFS/DFS 遍历。

  - 所有从 $S$ 可达的节点集合构成前景 (S-set)。

  - 其余不可达的节点构成背景 (T-set)。

完整代码如下：

#raw(block: true, lang: "python", read("assets/main.py"))

= 运行结果

在终端运行代码：

#figure(
  caption: [终端运行结果],
  image("assets/images/run-script.png"),
)

结果如下：

#grid(
  columns: 2,
  figure(
    caption: [原始图像],
    image("assets/images/hand.png"),
  ),
  figure(
    caption: [图像分割结果],
    image("assets/images/output.png"),
  ),
)