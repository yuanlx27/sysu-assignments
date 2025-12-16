use std::time::Instant;

const STUDENT_ID: u64 = 23336294;

#[derive(Copy, Clone, Debug, Default, PartialEq, Eq)]
struct U256 {
    d: [u64; 4],
}

impl U256 {
    fn new(v: u64) -> Self {
        let mut d = [0u64; 4];
        d[0] = v;
        Self { d }
    }
}

static mut P: U256 = U256 { d: [0; 4] };
static mut N: U256 = U256 { d: [0; 4] };
static mut ALPHA_BASE: U256 = U256 { d: [0; 4] };
static mut BETA: U256 = U256 { d: [0; 4] };
static mut R2: U256 = U256 { d: [0; 4] };
static mut ONE_MONT: U256 = U256 { d: [0; 4] };
static mut P_INV: u64 = 0;

fn from_string(s: &str) -> U256 {
    let mut res = U256::default();
    for c in s.chars() {
        let digit = c.to_digit(10).unwrap() as u64;
        let mut carry = digit;
        for i in 0..4 {
            let tmp = (res.d[i] as u128) * 10u128 + carry as u128;
            res.d[i] = tmp as u64;
            carry = (tmp >> 64) as u64;
        }
    }
    res
}

fn cmp_ge(a: &U256, b: &U256) -> bool {
    for i in (0..4).rev() {
        if a.d[i] > b.d[i] {
            return true;
        } else if a.d[i] < b.d[i] {
            return false;
        }
    }
    true
}

fn add_mod(mut a: U256, b: U256) -> U256 {
    let mut carry: u64 = 0;
    for i in 0..4 {
        let sum = a.d[i] as u128 + b.d[i] as u128 + carry as u128;
        a.d[i] = sum as u64;
        carry = (sum >> 64) as u64;
    }
    let p = unsafe { P };
    if cmp_ge(&a, &p) {
        sub_raw(a, p)
    } else {
        a
    }
}

fn sub_raw(mut a: U256, b: U256) -> U256 {
    let mut borrow: u64 = 0;
    for i in 0..4 {
        let bi = b.d[i].wrapping_add(borrow);
        if a.d[i] < bi {
            a.d[i] = a.d[i].wrapping_sub(bi);
            borrow = 1;
        } else {
            a.d[i] = a.d[i].wrapping_sub(bi);
            borrow = 0;
        }
    }
    a
}

fn mul_mont(a: U256, b: U256) -> U256 {
    let mut t = [0u64; 8];
    let p = unsafe { P };
    let p_inv = unsafe { P_INV };

    for i in 0..3 {
        let mut carry: u64 = 0;
        for j in 0..3 {
            let prod = a.d[i] as u128 * b.d[j] as u128 + t[i + j] as u128 + carry as u128;
            t[i + j] = prod as u64;
            carry = (prod >> 64) as u64;
        }
        let mut k = i + 3;
        while carry != 0 {
            let sum = t[k] as u128 + carry as u128;
            t[k] = sum as u64;
            carry = (sum >> 64) as u64;
            k += 1;
        }
    }

    for i in 0..3 {
        let u = t[i].wrapping_mul(p_inv);
        let mut carry: u64 = 0;
        for j in 0..3 {
            let prod = u as u128 * p.d[j] as u128 + t[i + j] as u128 + carry as u128;
            t[i + j] = prod as u64;
            carry = (prod >> 64) as u64;
        }
        let mut k = i + 3;
        while carry != 0 {
            let sum = t[k] as u128 + carry as u128;
            t[k] = sum as u64;
            carry = (sum >> 64) as u64;
            k += 1;
        }
    }

    let mut res = U256::default();
    res.d[0] = t[3];
    res.d[1] = t[4];
    res.d[2] = t[5];
    res.d[3] = 0;

    if cmp_ge(&res, &p) {
        sub_raw(res, p)
    } else {
        res
    }
}

fn pow_mont(mut base: U256, mut exp: u64) -> U256 {
    unsafe {
        let mut res = ONE_MONT;
        while exp > 0 {
            if exp & 1 == 1 {
                res = mul_mont(res, base);
            }
            base = mul_mont(base, base);
            exp >>= 1;
        }
        res
    }
}

fn init() {
    unsafe {
        P = from_string("3768901521908407201157691198029711972876087647970824596533");
        N = from_string("9993115456385501509");
        ALPHA_BASE =
            from_string("3107382411142271813235322646657672922264748410711464860476");
        BETA =
            from_string("2120553873612439845419858696451540936395844505496867133711");

        let p0 = P.d[0];
        let mut x: u64 = 1;
        for _ in 0..6 {
            x = x.wrapping_mul(2u64.wrapping_sub(p0.wrapping_mul(x)));
        }
        P_INV = x.wrapping_neg();

        let base = U256::new(2);
        R2 = base;
        for _ in 0..383 {
            R2 = add_mod(R2, R2);
        }

        ONE_MONT = U256::new(1);
        for _ in 0..192 {
            ONE_MONT = add_mod(ONE_MONT, ONE_MONT);
        }
    }
}

fn compute_alpha() {
    unsafe {
        let factors: [u64; 7] = [
            2,
            2,
            23,
            8783,
            2419781956425763,
            192888768642311611,
            STUDENT_ID,
        ];

        let mut a = mul_mont(ALPHA_BASE, R2);
        for f in factors.iter() {
            a = pow_mont(a, *f);
        }
        ALPHA_BASE = a;
        BETA = mul_mont(BETA, R2);
    }
}

fn gcd(mut a: u64, mut b: u64) -> u64 {
    while b != 0 {
        let r = a % b;
        a = b;
        b = r;
    }
    a
}

fn mod_inverse(a: u64, m: u64) -> u64 {
    let (m0, mut x0, mut x1) = (m as i128, 0i128, 1i128);
    if m == 1 {
        return 0;
    }
    let mut mm = m as i128;
    let mut aa = a as i128;
    while aa > 1 {
        let q = aa / mm;
        let mut t = mm;
        mm = aa % mm;
        aa = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    if x1 < 0 {
        x1 += m0;
    }
    (x1 as u64) % m
}

fn solve() {
    unsafe {
        let alpha = ALPHA_BASE;
        let beta = BETA;
        let n = N.d[0];

        const BRANCHES: usize = 32;
        let mut m = [U256::default(); BRANCHES];
        let mut a_inc = [0u64; BRANCHES];
        let mut b_inc = [0u64; BRANCHES];

        for i in 0..BRANCHES {
            a_inc[i] = (i as u64) + 1;
            b_inc[i] = (i as u64 * i as u64) + 1;

            let t1 = pow_mont(alpha, a_inc[i]);
            let t2 = pow_mont(beta, b_inc[i]);
            m[i] = mul_mont(t1, t2);
        }

        const TABLE_SIZE: usize = 1 << 20;
        #[derive(Clone, Default)]
        struct Entry {
            x: U256,
            a: u64,
            b: u64,
            valid: bool,
        }
        let mut table = vec![Entry::default(); TABLE_SIZE];

        let dp_mask: u64 = 0xFFFFF;

        let mut x = ONE_MONT;
        let mut a = 0u64;
        let mut b = 0u64;

        println!("Starting Pollard's Rho...");
        let start_time = Instant::now();

        let check = pow_mont(ONE_MONT, 12345);
        if check != ONE_MONT {
            println!("Error: 1^k != 1");
        }

        let mut steps: u64 = 0;
        loop {
            steps += 1;
            if steps % 1_000_000_000 == 0 {
                println!("Steps: {}", steps);
            }

            let idx = (x.d[0] & (BRANCHES as u64 - 1)) as usize;
            x = mul_mont(x, m[idx]);
            a = (a + a_inc[idx]) % n;
            b = (b + b_inc[idx]) % n;

            if (x.d[0] & dp_mask) == 0 {
                let mut hash = ((x.d[0] >> 16) as usize) & (TABLE_SIZE - 1);
                let mut probes = 0usize;
                while table[hash].valid && probes < TABLE_SIZE {
                    if table[hash].x == x {
                        let aa = table[hash].a;
                        let bb = table[hash].b;
                        let u = if a >= aa { a - aa } else { n - (aa - a) };
                        let v = if bb >= b { bb - b } else { n - (b - bb) };

                        if v == 0 {
                            hash = (hash + 1) & (TABLE_SIZE - 1);
                            probes += 1;
                            continue;
                        }

                        let g = gcd(v, n);
                        if g != 1 {
                            hash = (hash + 1) & (TABLE_SIZE - 1);
                            probes += 1;
                            continue;
                        }

                        let v_inv = mod_inverse(v, n);
                        let res = (u as u128 * v_inv as u128) % n as u128;
                        let x_log = res as u64;

                        let elapsed = start_time.elapsed();
                        println!("Found collision!");
                        println!("Result x: {}", x_log);
                        println!("Iterations: {}", steps);
                        println!("Time: {:.3} seconds", elapsed.as_secs_f64());

                        let verify = pow_mont(alpha, x_log);
                        if verify == beta {
                            println!("Verification Successful: alpha^x == beta");
                        } else {
                            println!("Verification Failed!");
                        }
                        return;
                    }
                    hash = (hash + 1) & (TABLE_SIZE - 1);
                    probes += 1;
                }

                if probes < TABLE_SIZE {
                    table[hash] = Entry {
                        x,
                        a,
                        b,
                        valid: true,
                    };
                } else {
                    println!("Table full!");
                }
            }
        }
    }
}

fn main() {
    init();
    compute_alpha();
    solve();
}

