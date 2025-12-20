#import "@local/sysu-templates:0.3.0": baogao

#show: baogao.with(
  title: "实验六：OSPF 动态路由协议",
  subtitle: "计算机网络实验成员心得体会",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 本人承担的工作

前期 debug；加入 PC3 后路由器 R1 的配置；以及用 #link("https://typst.app")[Typst] 实现的实验报告基础框架．

= 遇到的困难与解决方法

+ 测试连通性时从 R2 发出的 `tracert` 命令只能到达 R1 而无法到达 PC1．

  经排查发现问题为 PC1 的默认网关配置错误，正确的默认网关应为 R1 的 G0/0/1 接口地址，修改后问题解决．

= 体会与总结

实验务必注意细节．
