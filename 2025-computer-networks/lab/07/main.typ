#import "@local/sysu-templates:0.3.0": baogao

#show: baogao.with(
  title: "实验七：扩展访问控制列表",
  subtitle: "计算机网络实验成员心得体会",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 本人承担的工作

路由器 RTA 的端口配置，OSPF 配置，以及 ACL 配置．

= 遇到的困难与解决方法

+ 配置过程中多次忘记具体命令．

  查阅指导手册，或使用 `?` 在终端中查看．

+ 一开始配置 ACL 时没有看清楚要求，配置了错误的规则．

  使用 `undo acl` 删除错误的规则并重新配置．

= 体会与总结

实验务必注意细节．
