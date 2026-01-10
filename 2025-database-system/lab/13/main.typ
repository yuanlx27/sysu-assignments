#import "@local/sysu-templates:0.2.0": report

#show: report.with(
  title: "实验十三：数据库备份与恢复",
  subtitle: "数据库系统实验报告",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 实验目的

熟悉数据库的保护措施数据库备份与恢复。通过本次实验使读者在掌握备份和恢复的基本概念的基础上，掌握在 MySQL 中进行的各种备份和恢复的基本方式和方法。

= 实验环境

+ 操作系统：Arch Linux 6.17.9

+ 数据库管理系统：MariaDB 12.1.2 in Podman 5.7.1

= 实验内容

针对教学管理系统（jxgl）做如下操作：

+ 对表 sc、course 和 student 分别做表备份和表恢复。

  可以利用 `mysqldump`（MariaDB 使用 `mariadb-dump`）命令，它可以将数据库的信息以 SQL 语句的形式输出到 stdout，我们可以重定向到一个文件中保存起来作为备份。具体操作如下：

  #figure(
    caption: [备份 sc、course 和 student 表],
    image("assets/images/20260110-135337.png"),
  )

  恢复时可以利用 `mariadb` 命令执行备份文件中的 SQL 语句，从而恢复表的数据。具体操作如下：

  #figure(
    caption: [恢复 sc、course 和 student 表],
    image("assets/images/20260110-135434.png"),
  )

+ 对教学管理系统采用不同方法进行完整备份和恢复。

  对整个数据库的备份和恢复同理，我们甚至还可以利用 `date` 命令给备份文件加上时间戳。具体操作如下：

  #figure(
    caption: [备份 jxgl 数据库],
    image("assets/images/20260110-135817.png"),
  )

  #figure(
    caption: [恢复 jxgl 数据库],
    image("assets/images/20260110-140130.png"),
  )

  另外一种方法是直接物理备份和恢复（使用文件系统复制）数据库对应的文件，这种方法要求数据库在备份时处于关闭状态。由于我们的数据库运行在 Podman Container 中，不便直接操作其文件系统，因此这里不作演示。

+ 对教学管理系统于 2011 年 6 月 29 日上午 9:00:00 进行了差异备份；上午 9:40 数据库发生故障，根据其差异备份和日志文件进行位置恢复。

  只需仿照教材例 13-7 或例 13-8 的步骤操作即可。我们的 MariaDB Server 关闭了日志功能，因此这里不作演示。

  #figure(
    caption: [MariaDB Server 日志功能已关闭],
    image("assets/images/20260110-141714.png"),
  )

= 实验总结

熟悉了数据库的保护措施数据库备份与恢复。在掌握备份和恢复的基本概念的基础上，掌握了在 MySQL 中进行的各种备份和恢复的基本方式和方法。
