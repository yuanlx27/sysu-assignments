#import "@local/sysu-templates:0.2.0": report

#show: report.with(
  title: "实验七：数据库存储与优化",
  subtitle: "数据库系统实验报告",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 实验目的

了解不同实用数据库系统数据存放的存储介质情况、数据库与数据文件的存储结构与存取方式（尽可能查阅相关资料及系统联机帮助等），实践索引的使用效果，实践数据库系统的效率和调节。

= 实验环境

+ 操作系统：Arch Linux 6.17.9

+ 数据库管理系统：MariaDB 12.1.2 in Podman 5.7.1

= 实验内容

索引的使用效果测试。参照实验示例上机操作，增大 test 表的记录到 8 万条或更多，重做实验。多次记录耗时，并作分析比较。

== 构建测试表

运行下面的 SQL 代码：

#raw(block: true, lang: "sql", read("assets/1-init.sql"))

向 test 表插入 80000 条记录。之后可以用

#raw(block: true, lang: "sql", read("assets/1-test.sql"))

来验证表中的记录数。运行结果如下：

#figure(
  caption: [验证 test 表中的记录数],
  image("assets/images/20251223-150600.png"),
)

从 `Query OK, 80000 rows affected` 以及 `source assets/02.sql` 语句的输出可以看出，表中已经成功插入了 80000 条记录。

== 未建立索引时的性能基准测试

在没有任何索引的情况下执行基础操作，记录基准耗时。

+ 单记录插入：

  #raw(block: true, lang: "sql", read("assets/2-insert.sql"))

  运行结果如下：

  #figure(
    caption: [单记录插入],
    image("assets/images/20251223-150708.png"),
  )

+ 查询所有记录，按 `id` 排序：

  #raw(block: true, lang: "sql", read("assets/2-query-id.sql"))

  运行结果如下：

  #figure(
    caption: [按 `id` 排序查询所有记录],
    image("assets/images/20251223-150724.png"),
  )

+ 查询所有记录，按 `mm` 排序：

  #raw(block: true, lang: "sql", read("assets/2-query-mm.sql"))

  运行结果如下：

  #figure(
    caption: [按 `mm` 排序查询所有记录],
    image("assets/images/20251223-150747.png"),
  )

+ 单记录查询：

  #raw(block: true, lang: "sql", read("assets/2-query-single.sql"))

  运行结果如下：

  #figure(
    caption: [单记录查询],
    image("assets/images/20251223-150755.png"),
  )

== 对 `id` 字段建立非聚集索引

+ 建立索引：

  #raw(block: true, lang: "sql", read("assets/3-create.sql"))

  运行结果如下：

  #figure(
    caption: [对 `id` 建立非聚集索引],
    image("assets/images/20251223-150850.png"),
  )

+ 执行四种基础操作（命令同上），并记录耗时：

  #figure(
    caption: [单记录插入],
    image("assets/images/20251223-150859.png"),
  )

  #figure(
    caption: [按 `id` 排序查询所有记录],
    image("assets/images/20251223-150908.png"),
  )

  #figure(
    caption: [按 `mm` 排序查询所有记录],
    image("assets/images/20251223-150917.png"),
  )

  #figure(
    caption: [单记录查询],
    image("assets/images/20251223-150926.png"),
  )

+ 删除索引：

  #raw(block: true, lang: "sql", read("assets/3-delete.sql"))

  运行结果如下：

  #figure(
    caption: [删除对 `id` 建立的非聚集索引],
    image("assets/images/20251223-151043.png"),
  )

== 对 `mm` 字段建立非聚集索引

+ 建立索引：

  #raw(block: true, lang: "sql", read("assets/4-create.sql"))

  运行结果如下：

  #figure(
    caption: [对 `mm` 建立非聚集索引],
    image("assets/images/20251223-151125.png"),
  )

+ 执行四种基础操作（命令同上），并记录耗时：

  #figure(
    caption: [单记录插入],
    image("assets/images/20251223-153711.png"),
  )

  #figure(
    caption: [按 `id` 排序查询所有记录],
    image("assets/images/20251223-153724.png"),
  )

  #figure(
    caption: [按 `mm` 排序查询所有记录],
    image("assets/images/20251223-153731.png"),
  )

  #figure(
    caption: [单记录查询],
    image("assets/images/20251223-153741.png"),
  )

+ 删除索引：

  #raw(block: true, lang: "sql", read("assets/4-delete.sql"))

  运行结果如下：

  #figure(
    caption: [删除对 `mm` 建立的非聚集索引],
    image("assets/images/20251223-153840.png"),
  )

== 对 `id` 字段建立唯一索引

+ 建立索引：

  #raw(block: true, lang: "sql", read("assets/5-create.sql"))

  运行结果如下：

  #figure(
    caption: [对 `id` 字段建立唯一索引],
    image("assets/images/20251223-153910.png"),
  )

+ 执行四种基础操作（命令同上），并记录耗时：

  #figure(
    caption: [单记录插入],
    image("assets/images/20251223-153922.png"),
  )

  #figure(
    caption: [按 `id` 排序查询所有记录],
    image("assets/images/20251223-153929.png"),
  )

  #figure(
    caption: [按 `mm` 排序查询所有记录],
    image("assets/images/20251223-153936.png"),
  )

  #figure(
    caption: [单记录查询],
    image("assets/images/20251223-153950.png"),
  )

+ 删除索引：

  #raw(block: true, lang: "sql", read("assets/5-delete.sql"))

  运行结果如下：

  #figure(
    caption: [删除对 `id` 建立的唯一索引],
    image("assets/images/20251223-154008.png"),
  )

== 结论

+ 索引建立成本：建立和删除索引（尤其是唯一索引）需要消耗显著的时间，这在生产环境下对大数据量表操作时需格外谨慎。

+ 查询与排序：在本实验 8 万条数据的规模下，索引对 `ORDER BY` 的加速效果在毫秒级体现。

+ 唯一索引的开销：唯一索引在插入记录时由于需要进行唯一性校验，其插入耗时略高于普通非聚集索引。

= 实验总结

了解了不同实用数据库系统数据存放的存储介质情况、数据库与数据文件的存储结构与存取方式（尽可能查阅相关资料及系统联机帮助等），实践了索引的使用效果，实践了数据库系统的效率和调节。
