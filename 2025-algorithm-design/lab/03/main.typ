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
