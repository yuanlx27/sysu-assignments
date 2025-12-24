use rand::{Rng, rng};
use std::io::{self, Read, Write};

/// --- SHA-256 Implementation ---
/// Required to hash the message before signing.
struct Sha256 {
    state: [u32; 8],
    buffer: Vec<u8>,
    len: u64,
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

    fn new() -> Self {
        Self {
            state: [
                0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
            ],
            buffer: Vec::new(),
            len: 0,
        }
    }

    fn update(&mut self, data: &[u8]) {
        self.len += (data.len() as u64) * 8;
        self.buffer.extend_from_slice(data);
        while self.buffer.len() >= 64 {
            let chunk: Vec<u8> = self.buffer.drain(..64).collect();
            self.process_block(&chunk);
        }
    }

    fn finalize(mut self) -> [u8; 32] {
        let bit_len = self.len;
        self.buffer.push(0x80);
        while (self.buffer.len() + 8) % 64 != 0 {
            self.buffer.push(0);
        }
        self.buffer.extend_from_slice(&bit_len.to_be_bytes());
        
        let final_data = std::mem::take(&mut self.buffer);
        for chunk in final_data.chunks_exact(64) {
            self.process_block(chunk);
        }

        let mut out = [0u8; 32];
        for i in 0..8 {
            out[i * 4..(i + 1) * 4].copy_from_slice(&self.state[i].to_be_bytes());
        }
        out
    }

    fn process_block(&mut self, block: &[u8]) {
        let mut w = [0u32; 64];
        for i in 0..16 {
            w[i] = u32::from_be_bytes(block[i * 4..(i + 1) * 4].try_into().unwrap());
        }
        for i in 16..64 {
            let s0 = w[i - 15].rotate_right(7) ^ w[i - 15].rotate_right(18) ^ (w[i - 15] >> 3);
            let s1 = w[i - 2].rotate_right(17) ^ w[i - 2].rotate_right(19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16].wrapping_add(s0).wrapping_add(w[i - 7]).wrapping_add(s1);
        }
        let [mut a, mut b, mut c, mut d, mut e, mut f, mut g, mut h] = self.state;
        for i in 0..64 {
            let s1 = e.rotate_right(6) ^ e.rotate_right(11) ^ e.rotate_right(25);
            let ch = (e & f) ^ ((!e) & g);
            let temp1 = h.wrapping_add(s1).wrapping_add(ch).wrapping_add(Self::K[i]).wrapping_add(w[i]);
            let s0 = a.rotate_right(2) ^ a.rotate_right(13) ^ a.rotate_right(22);
            let maj = (a & b) ^ (a & c) ^ (b & c);
            let temp2 = s0.wrapping_add(maj);
            h = g; g = f; f = e;
            e = d.wrapping_add(temp1);
            d = c; c = b; b = a;
            a = temp1.wrapping_add(temp2);
        }
        let vals = [a, b, c, d, e, f, g, h];
        for i in 0..8 { self.state[i] = self.state[i].wrapping_add(vals[i]); }
    }
}

/// --- BigUint Implementation ---
/// Handles modular arithmetic for 2048-bit numbers.
#[derive(Clone, Debug, PartialEq, Eq, PartialOrd, Ord)]
struct BigUint {
    limbs: Vec<u64>, // Little-endian representation
}

impl BigUint {
    fn from_be_bytes(bytes: &[u8]) -> Self {
        let mut limbs = Vec::new();
        for chunk in bytes.rchunks(8) {
            let mut buf = [0u8; 8];
            let start = 8 - chunk.len();
            buf[start..].copy_from_slice(chunk);
            limbs.push(u64::from_be_bytes(buf));
        }
        Self::trim(Self { limbs })
    }

    fn to_be_bytes(&self, out_len: usize) -> Vec<u8> {
        let mut res = vec![0u8; out_len];
        for i in 0..(out_len / 8) {
            let val = self.limbs.get(i).copied().unwrap_or(0);
            let bytes = val.to_be_bytes();
            let pos = out_len - (i + 1) * 8;
            res[pos..pos + 8].copy_from_slice(&bytes);
        }
        res
    }

    fn trim(mut self) -> Self {
        while self.limbs.len() > 1 && self.limbs.last() == Some(&0) {
            self.limbs.pop();
        }
        if self.limbs.is_empty() { self.limbs.push(0); }
        self
    }

    fn is_zero(&self) -> bool { self.limbs.iter().all(|&x| x == 0) }

    fn add(&self, other: &Self) -> Self {
        let mut limbs = Vec::new();
        let mut carry = 0u128;
        let len = self.limbs.len().max(other.limbs.len());
        for i in 0..len {
            let a = self.limbs.get(i).copied().unwrap_or(0) as u128;
            let b = other.limbs.get(i).copied().unwrap_or(0) as u128;
            let sum = a + b + carry;
            limbs.push(sum as u64);
            carry = sum >> 64;
        }
        if carry > 0 { limbs.push(carry as u64); }
        Self { limbs }.trim()
    }

    fn sub(&self, other: &Self) -> Self {
        let mut limbs = Vec::new();
        let mut borrow = 0i128;
        for i in 0..self.limbs.len() {
            let a = self.limbs[i] as i128;
            let b = other.limbs.get(i).copied().unwrap_or(0) as i128;
            let diff = a - b - borrow;
            if diff < 0 {
                limbs.push((diff + (1 << 64)) as u64);
                borrow = 1;
            } else {
                limbs.push(diff as u64);
                borrow = 0;
            }
        }
        Self { limbs }.trim()
    }

    fn mul(&self, other: &Self) -> Self {
        let mut res_limbs = vec![0u64; self.limbs.len() + other.limbs.len()];
        for i in 0..self.limbs.len() {
            let mut carry = 0u128;
            for j in 0..other.limbs.len() {
                let prod = (self.limbs[i] as u128) * (other.limbs[j] as u128) + (res_limbs[i + j] as u128) + carry;
                res_limbs[i + j] = prod as u64;
                carry = prod >> 64;
            }
            res_limbs[i + other.limbs.len()] += carry as u64;
        }
        Self { limbs: res_limbs }.trim()
    }

    fn div_rem(&self, divisor: &Self) -> (Self, Self) {
        if divisor.is_zero() { panic!("Division by zero"); }
        let mut quotient = Self { limbs: vec![0] };
        let mut remainder = Self { limbs: vec![0] };
        for i in (0..self.limbs.len() * 64).rev() {
            remainder = remainder.shl_1();
            if self.get_bit(i) { remainder.limbs[0] |= 1; }
            if &remainder >= divisor {
                remainder = remainder.sub(divisor);
                quotient.set_bit(i);
            }
        }
        (quotient.trim(), remainder.trim())
    }

    fn shl_1(&self) -> Self {
        let mut limbs = Vec::new();
        let mut carry = 0u64;
        for &limb in &self.limbs {
            limbs.push((limb << 1) | carry);
            carry = limb >> 63;
        }
        if carry > 0 { limbs.push(carry); }
        Self { limbs }.trim()
    }

    fn get_bit(&self, bit: usize) -> bool {
        let limb_idx = bit / 64;
        let bit_idx = bit % 64;
        self.limbs.get(limb_idx).is_some_and(|&l| (l >> bit_idx) & 1 != 0)
    }

    fn set_bit(&mut self, bit: usize) {
        let limb_idx = bit / 64;
        let bit_idx = bit % 64;
        while self.limbs.len() <= limb_idx { self.limbs.push(0); }
        self.limbs[limb_idx] |= 1 << bit_idx;
    }

    fn mod_pow(&self, exp: &Self, m: &Self) -> Self {
        let mut res = Self::from_be_bytes(&[1]);
        let mut base = self.div_rem(m).1;
        for i in 0..(exp.limbs.len() * 64) {
            if exp.get_bit(i) {
                res = res.mul(&base).div_rem(m).1;
            }
            base = base.mul(&base).div_rem(m).1;
        }
        res
    }

    /// Modular inverse using Fermat's Little Theorem (q is prime).
    fn mod_inv(&self, q: &Self) -> Self {
        let two = Self::from_be_bytes(&[2]);
        let exp = q.sub(&two);
        self.mod_pow(&exp, q)
    }
}

/// --- DSA Signing Logic ---
fn dsa_sign(p: BigUint, q: BigUint, g: BigUint, x: BigUint, message: &[u8]) -> (Vec<u8>, Vec<u8>) {
    let mut hasher = Sha256::new();
    hasher.update(message);
    let z = BigUint::from_be_bytes(&hasher.finalize());
    let mut rng = rng();

    loop {
        // Generate random k, 0 < k < q
        let mut k_bytes = [0u8; 32];
        rng.fill(&mut k_bytes);
        let k = BigUint::from_be_bytes(&k_bytes).div_rem(&q).1;
        if k.is_zero() { continue; }

        let r = g.mod_pow(&k, &p).div_rem(&q).1;
        if r.is_zero() { continue; }

        let k_inv = k.mod_inv(&q);
        let xr = x.mul(&r);
        let s = k_inv.mul(&z.add(&xr)).div_rem(&q).1;

        if !s.is_zero() {
            return (r.to_be_bytes(32), s.to_be_bytes(32));
        }
    }
}

fn main() -> io::Result<()> {
    let mut stdin = io::stdin();
    let mut stdout = io::stdout();

    // 1. Read p (256 bytes)
    let mut p_buf = [0u8; 256];
    stdin.read_exact(&mut p_buf)?;
    let p = BigUint::from_be_bytes(&p_buf);

    // 2. Read q (32 bytes)
    let mut q_buf = [0u8; 32];
    stdin.read_exact(&mut q_buf)?;
    let q = BigUint::from_be_bytes(&q_buf);

    // 3. Read g (256 bytes)
    let mut g_buf = [0u8; 256];
    stdin.read_exact(&mut g_buf)?;
    let g = BigUint::from_be_bytes(&g_buf);

    // 4. Read x (32 bytes)
    let mut x_buf = [0u8; 32];
    stdin.read_exact(&mut x_buf)?;
    let x = BigUint::from_be_bytes(&x_buf);

    // 5. Read message length (uint16_t, big-endian)
    let mut len_buf = [0u8; 2];
    stdin.read_exact(&mut len_buf)?;
    let msg_len = u16::from_be_bytes(len_buf) as usize;

    // 6. Read message content
    let mut msg = vec![0u8; msg_len];
    stdin.read_exact(&mut msg)?;

    // Sign
    let (r, s) = dsa_sign(p, q, g, x, &msg);

    // 7. Write output (r and s, each 32 bytes)
    stdout.write_all(&r)?;
    stdout.write_all(&s)?;
    stdout.flush()?;

    Ok(())
}
