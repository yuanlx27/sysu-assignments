use std::fmt;
use std::cmp::Ordering;
use std::ops::{Add, AddAssign, Sub, SubAssign, Mul, MulAssign, Div, DivAssign, Rem, RemAssign, Shl, ShlAssign, Shr, ShrAssign};
use std::io::{self, Read, Write};
use rand::Rng;

// SHA-256 Implementation
pub struct Sha256 {
    state: [u32; 8],
    data: Vec<u8>,
    bit_len: u64,
}

impl Sha256 {
    const K: [u32; 64] = [
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    ];

    pub fn new() -> Self {
        Sha256 {
            state: [
                0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
            ],
            data: Vec::new(),
            bit_len: 0,
        }
    }

    pub fn update(&mut self, data: &[u8]) {
        self.data.extend_from_slice(data);
        self.bit_len += (data.len() as u64) * 8;
        self.process_blocks();
    }

    fn process_blocks(&mut self) {
        while self.data.len() >= 64 {
            let block: Vec<u8> = self.data.drain(0..64).collect();
            self.process_chunk(&block);
        }
    }

    fn process_chunk(&mut self, chunk: &[u8]) {
        let mut w = [0u32; 64];
        for i in 0..16 {
            w[i] = u32::from_be_bytes([chunk[i * 4], chunk[i * 4 + 1], chunk[i * 4 + 2], chunk[i * 4 + 3]]);
        }
        for i in 16..64 {
            let s0 = w[i - 15].rotate_right(7) ^ w[i - 15].rotate_right(18) ^ (w[i - 15] >> 3);
            let s1 = w[i - 2].rotate_right(17) ^ w[i - 2].rotate_right(19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16].wrapping_add(s0).wrapping_add(w[i - 7]).wrapping_add(s1);
        }

        let mut a = self.state[0];
        let mut b = self.state[1];
        let mut c = self.state[2];
        let mut d = self.state[3];
        let mut e = self.state[4];
        let mut f = self.state[5];
        let mut g = self.state[6];
        let mut h = self.state[7];

        for i in 0..64 {
            let s1 = e.rotate_right(6) ^ e.rotate_right(11) ^ e.rotate_right(25);
            let ch = (e & f) ^ (!e & g);
            let temp1 = h.wrapping_add(s1).wrapping_add(ch).wrapping_add(Self::K[i]).wrapping_add(w[i]);
            let s0 = a.rotate_right(2) ^ a.rotate_right(13) ^ a.rotate_right(22);
            let maj = (a & b) ^ (a & c) ^ (b & c);
            let temp2 = s0.wrapping_add(maj);

            h = g;
            g = f;
            f = e;
            e = d.wrapping_add(temp1);
            d = c;
            c = b;
            b = a;
            a = temp1.wrapping_add(temp2);
        }

        self.state[0] = self.state[0].wrapping_add(a);
        self.state[1] = self.state[1].wrapping_add(b);
        self.state[2] = self.state[2].wrapping_add(c);
        self.state[3] = self.state[3].wrapping_add(d);
        self.state[4] = self.state[4].wrapping_add(e);
        self.state[5] = self.state[5].wrapping_add(f);
        self.state[6] = self.state[6].wrapping_add(g);
        self.state[7] = self.state[7].wrapping_add(h);
    }

    pub fn finalize(mut self) -> [u8; 32] {
        let bit_len = self.bit_len;
        self.data.push(0x80);
        while (self.data.len() % 64) != 56 {
            self.data.push(0);
        }
        self.data.extend_from_slice(&bit_len.to_be_bytes());
        self.process_blocks();

        let mut result = [0u8; 32];
        for i in 0..8 {
            let bytes = self.state[i].to_be_bytes();
            result[i * 4] = bytes[0];
            result[i * 4 + 1] = bytes[1];
            result[i * 4 + 2] = bytes[2];
            result[i * 4 + 3] = bytes[3];
        }
        result
    }
}

pub fn dsa_verify(
    p: &BigInt,
    q: &BigInt,
    g: &BigInt,
    y: &BigInt,
    r: &BigInt,
    s: &BigInt,
    message: &[u8],
) -> bool {
    if *r <= BigInt::zero() || *r >= *q {
        return false;
    }
    if *s <= BigInt::zero() || *s >= *q {
        return false;
    }

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

    // v = (g^u1 * y^u2 mod p) mod q
    let gu1 = g.mod_pow(&u1, p);
    let yu2 = y.mod_pow(&u2, p);
    let v = (gu1 * yu2) % p.clone();
    let v = v % q.clone();

    v == *r
}

struct Montgomery {
    m: BigInt,
    rho: u64, // -m^{-1} mod 2^64
}

impl Montgomery {
    fn new(m: &BigInt) -> Self {
        // m must be odd
        if !m.test_bit(0) {
            panic!("Modulus must be odd for Montgomery multiplication");
        }
        let m0 = m.limbs[0];
        let mut inv = 1u64;
        // Newton-Raphson for inverse mod 2^64
        for _ in 0..6 {
            inv = inv.wrapping_mul(2u64.wrapping_sub(m0.wrapping_mul(inv)));
        }
        let rho = 0u64.wrapping_sub(inv);
        Montgomery { m: m.clone(), rho }
    }

    // Computes T * R^-1 mod m
    fn reduce(&self, t: &mut BigInt) {
        let n = self.m.limbs.len();
        if t.limbs.len() < 2 * n + 1 {
            t.limbs.resize(2 * n + 1, 0);
        }

        for i in 0..n {
            let u = t.limbs[i].wrapping_mul(self.rho);
            // t += u * m * 2^(64*i)
            let mut carry = 0u128;
            for j in 0..n {
                let product = (u as u128) * (self.m.limbs[j] as u128) + (t.limbs[i + j] as u128) + carry;
                t.limbs[i + j] = product as u64;
                carry = product >> 64;
            }
            
            let mut k = i + n;
            while carry > 0 {
                if k >= t.limbs.len() {
                    t.limbs.push(0);
                }
                let sum = (t.limbs[k] as u128) + carry;
                t.limbs[k] = sum as u64;
                carry = sum >> 64;
                k += 1;
            }
        }

        // t = t / R (shift right by n limbs)
        if t.limbs.len() > n {
            t.limbs.drain(0..n);
        } else {
            t.limbs.clear();
            t.limbs.push(0);
        }
        t.normalize();

        if *t >= self.m {
            *t -= self.m.clone();
        }
    }

    fn mul(&self, a: &BigInt, b: &BigInt) -> BigInt {
        let mut t = a * b;
        self.reduce(&mut t);
        t
    }
}

// A BigInt is represented as a vector of 64-bit integers (limbs).
// The limbs are stored in little-endian order (least significant limb first).
#[derive(Clone)]
pub struct BigInt {
    limbs: Vec<u64>,
}

impl BigInt {
    pub fn new() -> Self {
        BigInt { limbs: vec![0] }
    }

    pub fn from_u64(v: u64) -> Self {
        BigInt { limbs: vec![v] }
    }

    pub fn zero() -> Self {
        BigInt { limbs: vec![0] }
    }

    pub fn is_zero(&self) -> bool {
        self.limbs.iter().all(|&x| x == 0)
    }

    // Remove trailing zeros from the limbs vector
    fn normalize(&mut self) {
        while self.limbs.len() > 1 && self.limbs.last() == Some(&0) {
            self.limbs.pop();
        }
    }

    fn effective_len(&self) -> usize {
        for i in (0..self.limbs.len()).rev() {
            if self.limbs[i] != 0 {
                return i + 1;
            }
        }
        0
    }

    pub fn bit_len(&self) -> usize {
        let len = self.effective_len();
        if len == 0 {
            return 0;
        }
        let last = self.limbs[len - 1];
        (len - 1) * 64 + (64 - last.leading_zeros() as usize)
    }

    pub fn test_bit(&self, index: usize) -> bool {
        let limb_idx = index / 64;
        let bit_idx = index % 64;
        if limb_idx >= self.limbs.len() {
            return false;
        }
        (self.limbs[limb_idx] & (1 << bit_idx)) != 0
    }

    pub fn set_bit(&mut self, index: usize) {
        let limb_idx = index / 64;
        let bit_idx = index % 64;
        if limb_idx >= self.limbs.len() {
            self.limbs.resize(limb_idx + 1, 0);
        }
        self.limbs[limb_idx] |= 1 << bit_idx;
    }

    pub fn div_rem(&self, other: &Self) -> (Self, Self) {
        if other.is_zero() {
            panic!("division by zero");
        }
        if self < other {
            return (BigInt::zero(), self.clone());
        }
        if self == other {
            return (BigInt::from(1u64), BigInt::zero());
        }

        let mut quotient = BigInt::zero();
        let mut remainder = BigInt::zero();
        let bit_len = self.bit_len();

        for i in (0..bit_len).rev() {
            remainder = remainder << 1;
            if self.test_bit(i) {
                remainder.set_bit(0);
            }

            if remainder >= *other {
                remainder -= other.clone();
                quotient.set_bit(i);
            }
        }
        quotient.normalize();
        remainder.normalize();
        (quotient, remainder)
    }

    pub fn mod_pow(&self, exponent: &Self, modulus: &Self) -> Self {
        if modulus.is_zero() {
            panic!("modulus cannot be zero");
        }
        
        // Use Montgomery multiplication for odd modulus
        if modulus.test_bit(0) {
            let mont = Montgomery::new(modulus);
            let n_limbs = modulus.limbs.len();
            
            // Calculate R mod N. R = 2^(64 * n_limbs)
            let mut r = BigInt::zero();
            r.set_bit(n_limbs * 64);
            let r_mod_n = r % modulus.clone();
            
            // Convert base to Montgomery form: base_mont = base * R mod N
            let base_mont = (self.clone() * r_mod_n.clone()) % modulus.clone();
            
            let mut result_mont = r_mod_n; // 1 * R mod N
            let mut base_pow = base_mont;
            
            let exp_len = exponent.bit_len();
            for i in 0..exp_len {
                if exponent.test_bit(i) {
                    result_mont = mont.mul(&result_mont, &base_pow);
                }
                base_pow = mont.mul(&base_pow, &base_pow);
            }
            
            // Convert back: result = result_mont * R^-1 mod N
            let mut res = result_mont;
            mont.reduce(&mut res);
            res
        } else {
            let mut result = BigInt::from(1u64);
            let mut base = self.clone() % modulus.clone();
            let exp_len = exponent.bit_len();

            for i in 0..exp_len {
                if exponent.test_bit(i) {
                    result = (result * base.clone()) % modulus.clone();
                }
                base = (base.clone() * base) % modulus.clone();
            }
            result
        }
    }

    pub fn mod_inverse(&self, modulus: &Self) -> Option<Self> {
        let mut t = BigInt::zero();
        let mut newt = BigInt::from(1u64);
        let mut r = modulus.clone();
        let mut newr = self.clone();

        while !newr.is_zero() {
            let quotient = &r / &newr;
            
            let _temp_t = t.clone();
            
            let q_times_newt = &quotient * &newt;
            let q_times_newt_mod = q_times_newt % modulus.clone();
            
            let newt_next = if t >= q_times_newt_mod {
                (&t - &q_times_newt_mod) % modulus.clone()
            } else {
                (&t + modulus - q_times_newt_mod) % modulus.clone()
            };
            
            t = newt;
            newt = newt_next;

            let temp_r = r;
            r = newr;
            newr = temp_r - &quotient * &r;
        }

        if r > BigInt::from(1u64) {
            return None; // Not invertible
        }

        Some(t)
    }

    pub fn random(bits: usize) -> Self {
        let mut rng = rand::rng();
        let num_limbs = (bits + 63) / 64;
        let mut limbs = Vec::with_capacity(num_limbs);

        for _ in 0..num_limbs {
            limbs.push(rng.random());
        }

        // Mask the last limb to ensure exact bit length
        let excess_bits = num_limbs * 64 - bits;
        if excess_bits > 0 {
            limbs[num_limbs - 1] >>= excess_bits;
        }
        
        let mut res = BigInt { limbs };
        res.normalize();
        res
    }

    pub fn from_bytes_be(bytes: &[u8]) -> Self {
        let mut res = BigInt::zero();
        for &b in bytes {
            res = (res << 8) + BigInt::from(b as u64);
        }
        res
    }

    pub fn to_bytes_be(&self) -> Vec<u8> {
        if self.is_zero() {
            return vec![0];
        }
        let mut temp = self.clone();
        let mut bytes = Vec::new();
        while !temp.is_zero() {
            let (q, r) = temp.div_rem(&BigInt::from(256u64));
            bytes.push(r.limbs[0] as u8);
            temp = q;
        }
        bytes.reverse();
        bytes
    }
    
    pub fn to_bytes_be_pad(&self, len: usize) -> Vec<u8> {
        let mut bytes = self.to_bytes_be();
        if bytes.len() == 1 && bytes[0] == 0 {
             bytes = vec![];
        }
        if bytes.len() < len {
            let mut padded = vec![0u8; len - bytes.len()];
            padded.extend(bytes);
            padded
        } else if bytes.len() > len {
             bytes
        } else {
            bytes
        }
    }

    fn split_at_limb(&self, n: usize) -> (BigInt, BigInt) {
        let len = self.limbs.len();
        if n >= len {
            return (self.clone(), BigInt::zero());
        }
        let low = BigInt { limbs: self.limbs[0..n].to_vec() };
        let mut high = BigInt { limbs: self.limbs[n..].to_vec() };
        low.clone().normalize(); 
        let mut low = low;
        low.normalize();
        high.normalize();
        (low, high)
    }

    fn standard_mul(&self, other: &Self) -> Self {
        if self.is_zero() || other.is_zero() {
            return BigInt::zero();
        }

        let n = self.limbs.len();
        let m = other.limbs.len();
        let mut result_limbs = vec![0u64; n + m];

        for i in 0..n {
            let mut carry: u128 = 0;
            for j in 0..m {
                let product = (self.limbs[i] as u128) * (other.limbs[j] as u128)
                    + (result_limbs[i + j] as u128)
                    + carry;
                result_limbs[i + j] = product as u64;
                carry = product >> 64;
            }
            result_limbs[i + m] = carry as u64;
        }

        let mut result = BigInt { limbs: result_limbs };
        result.normalize();
        result
    }

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
}

impl Shl<usize> for BigInt {
    type Output = Self;

    fn shl(self, rhs: usize) -> Self {
        if rhs == 0 {
            return self;
        }
        let shift_limbs = rhs / 64;
        let shift_bits = rhs % 64;
        let mut new_limbs = vec![0; shift_limbs];
        
        if shift_bits == 0 {
            new_limbs.extend_from_slice(&self.limbs);
        } else {
            let mut carry = 0;
            for &limb in &self.limbs {
                let new_limb = (limb << shift_bits) | carry;
                new_limbs.push(new_limb);
                carry = limb >> (64 - shift_bits);
            }
            if carry > 0 {
                new_limbs.push(carry);
            }
        }
        
        let mut result = BigInt { limbs: new_limbs };
        result.normalize();
        result
    }
}

impl ShlAssign<usize> for BigInt {
    fn shl_assign(&mut self, rhs: usize) {
        *self = self.clone() << rhs;
    }
}

impl Shr<usize> for BigInt {
    type Output = Self;

    fn shr(self, rhs: usize) -> Self {
        if rhs == 0 {
            return self;
        }
        let shift_limbs = rhs / 64;
        if shift_limbs >= self.limbs.len() {
            return BigInt::zero();
        }
        
        let shift_bits = rhs % 64;
        let mut new_limbs = Vec::new();
        
        if shift_bits == 0 {
            new_limbs.extend_from_slice(&self.limbs[shift_limbs..]);
        } else {
            let mut carry = 0;
            for &limb in self.limbs[shift_limbs..].iter().rev() {
                let new_limb = (limb >> shift_bits) | carry;
                carry = limb << (64 - shift_bits);
                new_limbs.push(new_limb);
            }
            new_limbs.reverse();
        }
        
        let mut result = BigInt { limbs: new_limbs };
        result.normalize();
        result
    }
}

impl ShrAssign<usize> for BigInt {
    fn shr_assign(&mut self, rhs: usize) {
        *self = self.clone() >> rhs;
    }
}

impl Ord for BigInt {
    fn cmp(&self, other: &Self) -> Ordering {
        let len_self = self.effective_len();
        let len_other = other.effective_len();

        match len_self.cmp(&len_other) {
            Ordering::Equal => {
                for i in (0..len_self).rev() {
                    match self.limbs[i].cmp(&other.limbs[i]) {
                        Ordering::Equal => continue,
                        ord => return ord,
                    }
                }
                Ordering::Equal
            }
            ord => ord,
        }
    }
}

impl PartialOrd for BigInt {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl PartialEq for BigInt {
    fn eq(&self, other: &Self) -> bool {
        self.cmp(other) == Ordering::Equal
    }
}

impl Eq for BigInt {}

impl Add for BigInt {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        &self + &other
    }
}

impl<'a, 'b> Add<&'b BigInt> for &'a BigInt {
    type Output = BigInt;

    fn add(self, other: &'b BigInt) -> BigInt {
        let max_len = std::cmp::max(self.limbs.len(), other.limbs.len());
        let mut result_limbs = Vec::with_capacity(max_len + 1);
        let mut carry: u128 = 0;

        for i in 0..max_len {
            let a = if i < self.limbs.len() { self.limbs[i] as u128 } else { 0 };
            let b = if i < other.limbs.len() { other.limbs[i] as u128 } else { 0 };
            let sum = a + b + carry;
            result_limbs.push(sum as u64);
            carry = sum >> 64;
        }

        if carry > 0 {
            result_limbs.push(carry as u64);
        }

        let mut result = BigInt { limbs: result_limbs };
        result.normalize();
        result
    }
}

impl<'a> Add<&'a BigInt> for BigInt {
    type Output = BigInt;
    fn add(self, other: &'a BigInt) -> BigInt {
        &self + other
    }
}

impl AddAssign for BigInt {
    fn add_assign(&mut self, other: Self) {
        *self = &*self + &other;
    }
}

impl Sub for BigInt {
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        &self - &other
    }
}

impl<'a> Sub<&'a BigInt> for BigInt {
    type Output = BigInt;
    fn sub(self, other: &'a BigInt) -> BigInt {
        &self - other
    }
}

impl<'a, 'b> Sub<&'b BigInt> for &'a BigInt {
    type Output = BigInt;

    fn sub(self, other: &'b BigInt) -> BigInt {
        if self < other {
            panic!("BigInt subtraction underflow");
        }

        let mut result_limbs = Vec::with_capacity(self.limbs.len());
        let mut borrow: i128 = 0;

        for i in 0..self.limbs.len() {
            let a = self.limbs[i] as i128;
            let b = if i < other.limbs.len() { other.limbs[i] as i128 } else { 0 };
            let diff = a - b - borrow;

            if diff < 0 {
                result_limbs.push((diff + (1u128 << 64) as i128) as u64);
                borrow = 1;
            } else {
                result_limbs.push(diff as u64);
                borrow = 0;
            }
        }

        let mut result = BigInt { limbs: result_limbs };
        result.normalize();
        result
    }
}

impl SubAssign for BigInt {
    fn sub_assign(&mut self, other: Self) {
        *self = &*self - &other;
    }
}

impl Mul for BigInt {
    type Output = Self;

    fn mul(self, other: Self) -> Self {
        &self * &other
    }
}

impl<'a, 'b> Mul<&'b BigInt> for &'a BigInt {
    type Output = BigInt;

    fn mul(self, other: &'b BigInt) -> BigInt {
        self.karatsuba_mul(other)
    }
}

impl MulAssign for BigInt {
    fn mul_assign(&mut self, other: Self) {
        *self = &*self * &other;
    }
}

impl Div for BigInt {
    type Output = Self;

    fn div(self, other: Self) -> Self {
        self.div_rem(&other).0
    }
}

impl<'a, 'b> Div<&'b BigInt> for &'a BigInt {
    type Output = BigInt;

    fn div(self, other: &'b BigInt) -> BigInt {
        self.div_rem(other).0
    }
}

impl DivAssign for BigInt {
    fn div_assign(&mut self, other: Self) {
        *self = self.div_rem(&other).0;
    }
}

impl Rem for BigInt {
    type Output = Self;

    fn rem(self, other: Self) -> Self {
        self.div_rem(&other).1
    }
}

impl<'a, 'b> Rem<&'b BigInt> for &'a BigInt {
    type Output = BigInt;

    fn rem(self, other: &'b BigInt) -> BigInt {
        self.div_rem(other).1
    }
}

impl RemAssign for BigInt {
    fn rem_assign(&mut self, other: Self) {
        *self = self.div_rem(&other).1;
    }
}

impl From<u64> for BigInt {
    fn from(v: u64) -> Self {
        BigInt::from_u64(v)
    }
}

impl From<u32> for BigInt {
    fn from(v: u32) -> Self {
        BigInt::from_u64(v as u64)
    }
}

impl fmt::Debug for BigInt {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "BigInt(0x")?;
        if self.limbs.is_empty() {
            write!(f, "0")?;
        } else {
            for (i, limb) in self.limbs.iter().rev().enumerate() {
                if i == 0 {
                    write!(f, "{:x}", limb)?;
                } else {
                    write!(f, "{:016x}", limb)?;
                }
            }
        }
        write!(f, ")")
    }
}

impl fmt::Display for BigInt {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

fn main() -> io::Result<()> {
    let mut stdin = io::stdin();
    let mut stdout = io::stdout();

    // 1. p: 256 bytes
    let mut p_bytes = [0u8; 256];
    stdin.read_exact(&mut p_bytes)?;
    let p = BigInt::from_bytes_be(&p_bytes);

    // 2. q: 32 bytes
    let mut q_bytes = [0u8; 32];
    stdin.read_exact(&mut q_bytes)?;
    let q = BigInt::from_bytes_be(&q_bytes);

    // 3. g: 256 bytes
    let mut g_bytes = [0u8; 256];
    stdin.read_exact(&mut g_bytes)?;
    let g = BigInt::from_bytes_be(&g_bytes);

    // 4. y: 256 bytes (Public Key)
    let mut y_bytes = [0u8; 256];
    stdin.read_exact(&mut y_bytes)?;
    let y = BigInt::from_bytes_be(&y_bytes);

    // 5. r: 32 bytes
    let mut r_bytes = [0u8; 32];
    stdin.read_exact(&mut r_bytes)?;
    let r = BigInt::from_bytes_be(&r_bytes);

    // 6. s: 32 bytes
    let mut s_bytes = [0u8; 32];
    stdin.read_exact(&mut s_bytes)?;
    let s = BigInt::from_bytes_be(&s_bytes);

    // 7. msg_len: 2 bytes (uint16_t)
    let mut msg_len_bytes = [0u8; 2];
    stdin.read_exact(&mut msg_len_bytes)?;
    let msg_len = u16::from_le_bytes(msg_len_bytes) as usize;

    // 8. msg: msg_len bytes
    let mut msg = vec![0u8; msg_len];
    stdin.read_exact(&mut msg)?;

    if dsa_verify(&p, &q, &g, &y, &r, &s, &msg) {
        stdout.write_all(b"PASS")?;
    } else {
        stdout.write_all(b"FAIL")?;
    }

    Ok(())
}

