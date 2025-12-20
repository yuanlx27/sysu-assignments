#import "@local/sysu-templates:0.3.0": baogao

#show: baogao.with(
  title: "实验四：静态路由",
  subtitle: "计算机网络实验成员心得体会",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 本人承担的工作

连接网线，配置路由器 R1 和计算机 PC1．

= 遇到的困难与解决方法

+ 配置路由器 R1 时，执行命令 `interface Ethernet0/0/1` 失败．

  检查路由器接口后发现是千兆网口，改用 `interface GigabitEthernet0/0/1`，或简写为 `interface g0/0/1`．

+ 配置完路由器 R1 后，执行命令 `display ip routing-table` 输出中没有 `Static` 项．

  询问助教后得知需要根据具体的网口编号来执行对应的 `interface` 命令．

= 体会与总结

不能盲目地照着实验指导操作，要根据实际情况灵活调整．
