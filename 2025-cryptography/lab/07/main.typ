#import "@local/sysu-templates:0.2.0": report

#show: report.with(
  title: "实验七：DSA 签名的实现与验证",
  subtitle: "现代密码学实验报告",
  student: (name: "元朗曦", id: "23336294"),
  institude: "计算机学院",
  major: "计算机科学与技术",
  class: "计八",
)

= 实验目的

+ 通过从零开始实现 DSA 数字签名算法，深入理解公钥密码体制的设计原理与数学基础。

+ 通过手动实现大整数算术库（BigInt）和 SHA-256 哈希算法，掌握密码学底层运算的实现细节。

+ 通过引入 Karatsuba 乘法和 Montgomery 模幂等优化算法，提高对密码算法效率优化的认识和编程能力。

= 实验内容

使用 Rust 编程语言，在不依赖第三方大数库和加密库（仅允许使用 `rand` 生成随机数）的情况下，完成以下任务：

+ *大整数库实现*：实现支持 2048 位运算的大整数库 `BigInt`，支持加、减、乘、除、模、位移等基本运算。

+ *哈希算法实现*：依据 FIPS 180-4 标准实现 SHA-256 哈希算法。

+ *DSA 签名生成 (Experiment 7-1)*：实现 DSA 签名算法，输入私钥和消息，输出签名 $(r, s)$。

+ *DSA 签名验证 (Experiment 7-2)*：实现 DSA 验签算法，输入公钥、消息和签名，输出验证结果。

*参数要求*：

- 模数 $p$ 长度为 2048 位。

- 子群阶 $q$ 长度为 256 位。

- 哈希函数使用 SHA-256。

= 实验原理

== DSA 签名算法

DSA (Digital Signature Algorithm) 是基于整数有限域离散对数难题的数字签名标准。

- *签名过程*：

  + 选取随机数 $k$ ($0 < k < q$)。

  + 计算 $r = (g^k mod p) mod q$。

  + 计算 $s = k^(-1)(H(m) + x dot r) mod q$。

- *验证过程*：

  + 计算 $w = s^(-1) mod q$。

  + 计算 $u_1 = H(m) dot w mod q$, $u_2 = r dot w mod q$。

  + 计算 $v = (g^(u_1) y^(u_2) mod p) mod q$。

  + 验证 $v = r$。

== 大整数运算优化

- *Karatsuba 乘法*：一种快速乘法算法。通过将大整数分块，将 $n$ 位数的乘法从 $O(n^2)$ 降低到 $O(n^(log_2 3)) approx O(n^1.585)$。

- *Montgomery 模幂*：一种在模算术中快速计算幂的方法。通过将数字转换到 Montgomery 域，将昂贵的除法取模操作转换为简单的移位和加法操作，显著加速 $g^k mod p$ 的计算。

= 实验步骤

由于代码较长，以下仅展示核心数据结构与关键算法实现。完整代码见提交记录。

== BigInt 结构体与基础定义

使用 ```rust Vec<u64>``` 存储大整数的肢体（Limbs），采用小端序。

```rust
#[derive(Clone)]
pub struct BigInt {
    limbs: Vec<u64>,
}

impl BigInt {
    // ... 基础构造与位操作 ...
}
```

== Karatsuba 乘法实现

当肢体数量超过阈值（32）时，使用 Karatsuba 算法递归计算。

```rust
fn karatsuba_mul(&self, other: &Self) -> Self {
    let n = self.limbs.len();
    let m = other.limbs.len();
    let len = std::cmp::max(n, m);
    
    if len <= 32 {
        return self.standard_mul(other);
    }
    
    let half = len / 2;
    let (low1, high1) = self.split_at_limb(half);
    let (low2, high2) = other.split_at_limb(half);
    
    let z0 = low1.karatsuba_mul(&low2);
    let z2 = high1.karatsuba_mul(&high2);
    
    let lh1 = &low1 + &high1;
    let lh2 = &low2 + &high2;
    let z1 = lh1.karatsuba_mul(&lh2) - &z0 - &z2;
    
    let shift_bits = half * 64;
    let p1 = z2 << (2 * shift_bits);
    let p2 = z1 << shift_bits;
    
    p1 + p2 + z0
}
```

== DSA 签名生成

```rust
pub fn dsa_sign(p: &BigInt, q: &BigInt, g: &BigInt, x: &BigInt, message: &[u8]) -> (BigInt, BigInt) {
    let mut hasher = Sha256::new();
    hasher.update(message);
    let hash = hasher.finalize();
    let h_m = BigInt::from_bytes_be(&hash);

    loop {
        let k = loop {
            let k_cand = BigInt::random(256);
            if k_cand > BigInt::zero() && k_cand < *q {
                break k_cand;
            }
        };

        let r = g.mod_pow(&k, p) % q.clone();
        if r.is_zero() { continue; }

        let k_inv = match k.mod_inverse(q) {
            Some(inv) => inv,
            None => continue,
        };

        let xr = (x.clone() * r.clone()) % q.clone();
        let hm_xr = (h_m.clone() + xr) % q.clone();
        let s = (k_inv * hm_xr) % q.clone();

        if s.is_zero() { continue; }

        return (r, s);
    }
}
```

== DSA 签名验证

```rust
pub fn dsa_verify(p: &BigInt, q: &BigInt, g: &BigInt, y: &BigInt, r: &BigInt, s: &BigInt, message: &[u8]) -> bool {
    if *r <= BigInt::zero() || *r >= *q { return false; }
    if *s <= BigInt::zero() || *s >= *q { return false; }

    let w = match s.mod_inverse(q) {
        Some(inv) => inv,
        None => return false,
    };

    let mut hasher = Sha256::new();
    hasher.update(message);
    let hash = hasher.finalize();
    let h_m = BigInt::from_bytes_be(&hash);

    let u1 = (h_m * w.clone()) % q.clone();
    let u2 = (r.clone() * w) % q.clone();

    let gu1 = g.mod_pow(&u1, p);
    let yu2 = y.mod_pow(&u2, p);
    let v = (gu1 * yu2) % p.clone();
    let v = v % q.clone();

    v == *r
}
```

= 实验结果

+ *编译构建*：项目在 Rust 2024 环境下成功编译，无警告。

+ *功能测试*：

  - 编写了单元测试验证 Karatsuba 乘法与标准乘法结果的一致性。

  - 编写了单元测试验证 Montgomery 模幂与标准模幂结果的一致性。

  - 签名生成程序能够读取二进制输入，并输出符合格式的 32 字节 $r$ 和 $s$。

  - 签名验证程序能够正确判断签名的有效性，输出 `PASS` 或 `FAIL`。

- *提交结果*：

  #grid(
    columns: 2,
    figure(
      caption: [DSA 签名生成提交结果],
      image("assets/images/submission-1.png"),
    ),
    figure(
      caption: [DSA 签名验证提交结果],
      image("assets/images/submission-2.png"),
    ),
  )

= 思考题

+ *为什么 DSA 签名中的随机数 $k$ 必须保密且不可重复使用？*

  *答*：随机数 $k$ 是 DSA 安全性的关键。如果 $k$ 泄露，攻击者可以通过公式 $x = r^{-1}(s k - H(m)) mod q$ 直接计算出私钥 $x$。如果 $k$ 被重复使用（即对两个不同的消息 $m_1, m_2$ 使用了相同的 $k$），攻击者可以通过两个签名 $(r, s_1)$ 和 $(r, s_2)$ 建立方程组，消去 $k$ 后解出私钥 $x$。因此，$k$ 必须是不可预测的随机数，且每次签名都必须重新生成。

+ *在实现大整数运算时，为什么要引入 Montgomery 算法？*

  *答*：DSA 签名和验证过程中涉及大量的模幂运算（如 $g^k mod p$）。在 2048 位的大整数下，普通的取模运算（除法）非常耗时。Montgomery 乘法通过将数字映射到 Montgomery 域，将复杂的除法取模操作转化为简单的移位和加法操作，极大地提高了模乘和模幂的效率，是提升 RSA、DSA 等公钥算法性能的关键技术。

= 实验总结

本次实验通过手写实现 DSA 及其依赖的底层大数运算库，让我深刻体会到了公钥密码学算法的实现细节。

+ *底层算术的重要性*：大整数库的性能直接决定了上层密码算法的运行效率。通过实现 Karatsuba 和 Montgomery 算法，我认识到了算法优化在工业级密码实现中的核心地位。

+ *安全性细节*：在实现 DSA 时，必须严格遵循标准（如 FIPS 186-4），特别是在随机数生成和边界检查（如验证 $0 < r < q$）方面，任何疏忽都可能导致安全漏洞。

+ *Rust 语言特性*：Rust 的所有权机制和强类型系统在处理大整数内存管理和类型转换时提供了很好的安全保障，减少了缓冲区溢出等低级错误的发生。