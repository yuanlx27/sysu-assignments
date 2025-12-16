#import "@local/sysu-templates:0.2.0": report

#show: report.with(
  title: "实验六：DLP 计算",
  subtitle: "现代密码学实验报告",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 实验目的

本实验旨在通过实现 Pollard Rho 算法，理解并掌握大素数模下的离散对数问题的求解方法，体会现代密码学中相关算法的实际应用。

= 实验原理

离散对数问题（DLP）是指：已知素数 $p$，生成元 $alpha$，以及 $beta = alpha^x mod p$，求 $x$。该问题在大素数模下被认为是计算困难的，是许多公钥密码体制（如 Diffie-Hellman 密钥交换、ElGamal 加密等）的安全基础。

Pollard Rho 算法是一种高效的离散对数求解方法，利用伪随机游走和生日悖论思想，通过检测碰撞来求解 $x$。其主要思想是构造状态转移函数，使得在有限状态空间内随机游走，最终出现碰撞（即两个不同路径到达同一状态），从而建立关于 $x$ 的线性同余方程，进而求解。

= 实验内容

+ 输入参数：

  - $p$：大素数

  - $n$：素数，$n <= 2^64 - 1$

  - $alpha$、$beta$：满足 $beta = alpha^x mod p$

+ 主要步骤：

  - 实现 `U256` 结构体，支持 256 位整数的加减、Montgomery 乘法等运算。

  - 实现模幂、模逆等基础数论运算。

  - 按照实验要求初始化 $p$、$n$、$alpha$、$beta$，并将 $alpha$ 变换为 $alpha^(product f_i)$，以保证 $alpha$ 的阶为 $n$。

  - 实现 Pollard Rho 算法，采用分支随机游走和哈希表检测碰撞，求解 $x$。

  - 输出碰撞信息、计算得到的 $x$、迭代次数和用时，并验证 $alpha^x equiv beta mod p$。

代码如下：

#raw(block: true, lang: "rust", read("code-1/src/main.rs"))

运行结果如下：

#figure(
  caption: [DLP 运行结果],
  image("assets/result.png"),
)

我们可以写一个简单的 Python 脚本，计算结果的 Argon2ID 哈希值，并与实验提供的标准答案进行对比，以验证正确性：

#raw(block: true, lang: "python", read("assets/check.py"))

运行结果如下：

#figure(
  caption: [哈希测试运行结果],
  image("assets/check-result.png"),
)

= 实验总结

本实验通过实现 Pollard Rho 算法，深入理解了离散对数问题的难度及其在密码学中的重要性。实验过程中，体会到大数运算、Montgomery 乘法等底层实现对算法效率的影响。通过分支随机游走和哈希表优化，算法能在较短时间内求解大素数模下的离散对数。实验加深了对现代密码学基础的理解，并提升了实际编程能力。