/*
 * DSA Signature Implementation in Rust
 * * Requirements:
 * - No external crates (except `rand` for nonce generation).
 * - Custom BigInt and SHA-256 implementation.
 * - Input parsing from Hex stream.
 * - Output formatting strictly matching the problem description.
 */

use std::io::{self, Read};
use rand::{Rng, RngCore, rngs::OsRng};

// ============================================================================
// PART 1: SHA-256 Implementation
// ============================================================================

#[derive(Clone)]
struct Sha256 {
    state: [u32; 8],
    data: [u8; 64],
    datalen: usize,
    bitlen: u64,
}

impl Sha256 {
    fn new() -> Sha256 {
        Sha256 {
            state: [
                0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
            ],
            data: [0; 64],
            datalen: 0,
            bitlen: 0,
        }
    }

    fn update(&mut self, data: &[u8]) {
        for &byte in data {
            self.data[self.datalen] = byte;
            self.datalen += 1;
            self.bitlen += 8;
            if self.datalen == 64 {
                self.transform();
                self.datalen = 0;
            }
        }
    }

    fn finalize(mut self) -> [u8; 32] {
        let i = self.datalen;
        self.data[i] = 0x80; // Append 1 bit
        self.datalen += 1;

        if self.datalen > 56 {
            while self.datalen < 64 {
                self.data[self.datalen] = 0;
                self.datalen += 1;
            }
            self.transform();
            self.datalen = 0;
        }

        while self.datalen < 56 {
            self.data[self.datalen] = 0;
            self.datalen += 1;
        }

        // Append length in bits (Big Endian)
        let bits = self.bitlen;
        self.data[56] = (bits >> 56) as u8;
        self.data[57] = (bits >> 48) as u8;
        self.data[58] = (bits >> 40) as u8;
        self.data[59] = (bits >> 32) as u8;
        self.data[60] = (bits >> 24) as u8;
        self.data[61] = (bits >> 16) as u8;
        self.data[62] = (bits >> 8) as u8;
        self.data[63] = (bits) as u8;
        self.transform();

        let mut out = [0u8; 32];
        for (i, &val) in self.state.iter().enumerate() {
            out[i * 4] = (val >> 24) as u8;
            out[i * 4 + 1] = (val >> 16) as u8;
            out[i * 4 + 2] = (val >> 8) as u8;
            out[i * 4 + 3] = val as u8;
        }
        out
    }

    fn transform(&mut self) {
        let mut w = [0u32; 64];
        for i in 0..16 {
            w[i] = u32::from_be_bytes([
                self.data[i * 4],
                self.data[i * 4 + 1],
                self.data[i * 4 + 2],
                self.data[i * 4 + 3],
            ]);
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

        let k: [u32; 64] = [
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
        ];

        for i in 0..64 {
            let s1 = e.rotate_right(6) ^ e.rotate_right(11) ^ e.rotate_right(25);
            let ch = (e & f) ^ ((!e) & g);
            let temp1 = h.wrapping_add(s1).wrapping_add(ch).wrapping_add(k[i]).wrapping_add(w[i]);
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
}

// ============================================================================
// PART 2: Custom BigInt Implementation (Optimized for performance)
// ============================================================================

// Stored as little-endian words (u64)
#[derive(Clone, Debug, PartialEq, Eq, PartialOrd, Ord)]
struct BigInt {
    data: Vec<u64>,
}

impl BigInt {
    fn new() -> Self {
        BigInt { data: vec![0] }
    }

    fn from_u64(v: u64) -> Self {
        if v == 0 {
            Self::new()
        } else {
            BigInt { data: vec![v] }
        }
    }

    // Parse from Big-Endian byte slice
    fn from_be_bytes(bytes: &[u8]) -> Self {
        let mut data = Vec::new();
        // Process chunks of 8 bytes from the end
        for chunk in bytes.rchunks(8) {
            let mut buf = [0u8; 8];
            let len = chunk.len();
            // Copy into end of buf (since it's big endian number, the last byte is LSB of that chunk)
            // But we want to interpret the chunk as a u64 BE
            buf[8 - len..].copy_from_slice(chunk);
            data.push(u64::from_be_bytes(buf));
        }
        let mut bi = BigInt { data };
        bi.trim();
        bi
    }

    fn to_be_bytes(&self) -> Vec<u8> {
        if self.is_zero() {
            return vec![0];
        }
        let mut bytes = Vec::new();
        // Push all full words
        for &word in self.data.iter() {
            bytes.extend_from_slice(&word.to_be_bytes());
        }
        // The vector is now Little Endian words, but bytes within words are Big Endian.
        // We need to reverse the order of words (chunks of 8) to get full Big Endian representation.
        // Actually simpler: iterate reverse
        bytes.clear();
        for &word in self.data.iter().rev() {
            bytes.extend_from_slice(&word.to_be_bytes());
        }
        
        // Remove leading zeros
        let start = bytes.iter().position(|&x| x != 0).unwrap_or(bytes.len() - 1);
        bytes[start..].to_vec()
    }
    
    // Specifically for the output requirement: 32 bytes fixed width Big Endian
    fn to_be_bytes_padded(&self, size: usize) -> Vec<u8> {
        let mut raw = self.to_be_bytes();
        if raw.len() > size {
            // If strictly larger, we might take the last 'size' bytes, but usually signifies overflow
            // For this problem, modulo arithmetic ensures we stay within bounds.
            raw = raw[raw.len()-size..].to_vec();
        }
        let mut out = vec![0u8; size];
        let offset = size - raw.len();
        out[offset..].copy_from_slice(&raw);
        out
    }

    fn trim(&mut self) {
        while self.data.len() > 1 && self.data.last() == Some(&0) {
            self.data.pop();
        }
    }

    fn is_zero(&self) -> bool {
        self.data.len() == 1 && self.data[0] == 0
    }

    // --- Arithmetic ---

    fn add(&self, other: &Self) -> Self {
        let len = std::cmp::max(self.data.len(), other.data.len());
        let mut result = Vec::with_capacity(len + 1);
        let mut carry = 0u128;
        
        for i in 0..len {
            let a = *self.data.get(i).unwrap_or(&0) as u128;
            let b = *other.data.get(i).unwrap_or(&0) as u128;
            let sum = a + b + carry;
            result.push(sum as u64);
            carry = sum >> 64;
        }
        if carry > 0 {
            result.push(carry as u64);
        }
        BigInt { data: result }
    }

    // Assumes self >= other
    fn sub(&self, other: &Self) -> Self {
        let mut result = Vec::with_capacity(self.data.len());
        let mut borrow = 0i128;
        
        for i in 0..self.data.len() {
            let a = *self.data.get(i).unwrap_or(&0) as i128;
            let b = *other.data.get(i).unwrap_or(&0) as i128;
            let diff = a - b - borrow;
            if diff < 0 {
                result.push((diff + (1u128 << 64) as i128) as u64);
                borrow = 1;
            } else {
                result.push(diff as u64);
                borrow = 0;
            }
        }
        let mut bi = BigInt { data: result };
        bi.trim();
        bi
    }

    fn mul(&self, other: &Self) -> Self {
        if self.is_zero() || other.is_zero() {
            return BigInt::new();
        }
        let n = self.data.len();
        let m = other.data.len();
        let mut result = vec![0u64; n + m];

        for i in 0..n {
            let mut carry = 0u128;
            for j in 0..m {
                let p = (self.data[i] as u128) * (other.data[j] as u128) 
                      + (result[i + j] as u128) + carry;
                result[i + j] = p as u64;
                carry = p >> 64;
            }
            result[i + m] = carry as u64;
        }
        let mut bi = BigInt { data: result };
        bi.trim();
        bi
    }

    // Returns (quotient, remainder)
    fn div_rem(&self, other: &Self) -> (Self, Self) {
        if other.is_zero() {
            panic!("Division by zero");
        }
        if self < other {
            return (BigInt::new(), self.clone());
        }
        if other.data.len() == 1 {
            // Optimization for small divisor
            return self.div_rem_u64(other.data[0]);
        }

        // Knuth's Algorithm D implementation
        // Normalize
        let shift = other.data.last().unwrap().leading_zeros();
        let u = self.shl(shift as usize);
        let v = other.shl(shift as usize);
        
        let m = u.data.len() - v.data.len();
        let n = v.data.len();
        
        // Ensure u has enough space (maybe m+n+1 words implicitly)
        let mut u_digits = u.data.clone();
        if u_digits.len() < n + m + 1 {
            u_digits.push(0); 
        }

        let mut q = vec![0u64; m + 1];
        let v_n_1 = v.data[n - 1] as u128;
        let v_n_2 = v.data[n - 2] as u128;

        for j in (0..=m).rev() {
            let u_jn = u_digits.get(j + n).copied().unwrap_or(0) as u128;
            let u_jn_1 = u_digits.get(j + n - 1).copied().unwrap_or(0) as u128;
            let u_jn_2 = u_digits.get(j + n - 2).copied().unwrap_or(0) as u128;

            // Estimate q_hat
            let dividend = (u_jn << 64) | u_jn_1;
            let mut q_hat = dividend / v_n_1;
            let mut r_hat = dividend % v_n_1;

            loop {
                if q_hat >= (1 << 64) || (q_hat * v_n_2 > (r_hat << 64) | u_jn_2) {
                    q_hat -= 1;
                    r_hat += v_n_1;
                    if r_hat >= (1 << 64) { break; } // prevent overflow in r_hat check logic? usually covered by condition
                } else {
                    break;
                }
            }

            // Multiply and subtract: u[j..j+n] -= q_hat * v
            // If result is negative, add back v and decrement q_hat
            let mut borrow = 0i128;
            for i in 0..n {
                let p = q_hat * (v.data[i] as u128);
                let sub = (u_digits[j + i] as i128) - (p as u64 as i128) - borrow;
                u_digits[j + i] = sub as u64;
                // Calculate borrow. Note: p >> 64 is the high part of mult.
                // But we handled low part. The standard subtraction borrow logic:
                borrow = (p >> 64) as i128;
                if sub < 0 {
                    borrow += 1;
                }
            }
            // Handle the top digit subtraction
            let sub = (u_digits[j + n] as i128) - borrow;
            u_digits[j + n] = sub as u64;
            
            // Correction
            if sub < 0 {
                q_hat -= 1;
                let mut carry = 0u128;
                for i in 0..n {
                    let sum = (u_digits[j + i] as u128) + (v.data[i] as u128) + carry;
                    u_digits[j + i] = sum as u64;
                    carry = sum >> 64;
                }
                u_digits[j + n] = u_digits[j + n].wrapping_add(carry as u64);
            }
            
            q[j] = q_hat as u64;
        }

        let mut rem = BigInt { data: u_digits };
        rem.trim();
        // Denormalize remainder
        rem = rem.shr(shift as usize);
        
        let mut quo = BigInt { data: q };
        quo.trim();
        
        (quo, rem)
    }

    fn div_rem_u64(&self, divisor: u64) -> (Self, Self) {
        let mut rem = 0u128;
        let mut quo = vec![0u64; self.data.len()];
        for i in (0..self.data.len()).rev() {
            let cur = (rem << 64) | (self.data[i] as u128);
            quo[i] = (cur / divisor as u128) as u64;
            rem = cur % divisor as u128;
        }
        let mut bi = BigInt { data: quo };
        bi.trim();
        (bi, BigInt::from_u64(rem as u64))
    }

    // --- Bitwise Operations ---

    fn shl(&self, shift: usize) -> Self {
        let word_shift = shift / 64;
        let bit_shift = shift % 64;
        let mut result = vec![0u64; self.data.len() + word_shift + 1];
        
        if bit_shift == 0 {
            for i in 0..self.data.len() {
                result[i + word_shift] = self.data[i];
            }
        } else {
            let mut carry = 0;
            for i in 0..self.data.len() {
                let val = self.data[i];
                result[i + word_shift] = (val << bit_shift) | carry;
                carry = val >> (64 - bit_shift);
            }
            result[self.data.len() + word_shift] = carry;
        }
        let mut bi = BigInt { data: result };
        bi.trim();
        bi
    }

    fn shr(&self, shift: usize) -> Self {
        if shift >= self.bit_len() {
            return BigInt::new();
        }
        let word_shift = shift / 64;
        let bit_shift = shift % 64;
        let mut result = Vec::new();
        
        if bit_shift == 0 {
            for i in word_shift..self.data.len() {
                result.push(self.data[i]);
            }
        } else {
            let mut carry = 0;
            for i in (word_shift..self.data.len()).rev() {
                let val = self.data[i];
                let new_val = (val >> bit_shift) | (carry << (64 - bit_shift));
                carry = val;
                result.push(new_val);
            }
            result.reverse();
        }
        let mut bi = BigInt { data: result };
        bi.trim();
        bi
    }

    fn bit_len(&self) -> usize {
        if self.is_zero() { return 0; }
        (self.data.len() - 1) * 64 + (64 - self.data.last().unwrap().leading_zeros() as usize)
    }

    // --- Modular Arithmetic ---

    fn mod_pow(&self, exp: &BigInt, modulus: &BigInt) -> BigInt {
        let mut res = BigInt::from_u64(1);
        let mut base = self.div_rem(modulus).1;
        let mut e = exp.clone();

        while !e.is_zero() {
            if e.data[0] % 2 == 1 {
                res = res.mul(&base).div_rem(modulus).1;
            }
            base = base.mul(&base).div_rem(modulus).1;
            e = e.shr(1);
        }
        res
    }

    fn mod_inverse(&self, modulus: &BigInt) -> BigInt {
        // Extended Euclidean Algorithm
        let mut t = BigInt::new();
        let mut newt = BigInt::from_u64(1);
        let mut r = modulus.clone();
        let mut newr = self.clone();

        while !newr.is_zero() {
            let (q, _) = r.div_rem(&newr);
            
            // (t, newt) = (newt, t - q * newt)
            let temp_t = t;
            let product = q.mul(&newt);
            // Handling subtraction of potentially larger numbers in modular arithmetic context
            // t_new = t - q*newt. If q*newt > t, we work in mod modulus
            // But standard extended euclidean works with integers. 
            // We use a property: result is modulo modulus.
            // Let's assume arithmetic works with signed, but BigInt is unsigned.
            // We'll calculate: if t < product, t = t + k*modulus - product.
            // Actually, simplest way for unsigned: track sign or use recursion.
            // Iterative approach with positive numbers:
            // We maintain t and r.
            // It's tricky with unsigned only.
            // Standard trick: `t` and `newt` can be treated modulo `modulus`.
            // new_val = (t - q*newt) mod m
            
            let q_times_newt_mod = product.div_rem(modulus).1;
            
            // t = t - q_times_newt_mod (mod m)
            let mut next_t = if temp_t >= q_times_newt_mod {
                temp_t.sub(&q_times_newt_mod)
            } else {
                temp_t.add(modulus).sub(&q_times_newt_mod)
            };
            // Ensure next_t is reduced
            if next_t >= *modulus {
                next_t = next_t.div_rem(modulus).1;
            }

            t = newt;
            newt = next_t;

            // (r, newr) = (newr, r - q * newr)
            let last_r = r;
            r = newr;
            // newr = last_r % newr, which is what div_rem gives as remainder
            // but we need to update it properly
            // Actually, `r - q*newr` IS the remainder of r / newr.
            newr = last_r.div_rem(&r).1;
        }

        if r > BigInt::from_u64(1) {
            panic!("Not invertible");
        }
        
        t
    }
}

// ============================================================================
// PART 3: Helper Functions & Main Logic
// ============================================================================

// Helper to convert hex string to byte vector
fn parse_hex_input(input: &str) -> Vec<u8> {
    let hex_chars: String = input.chars().filter(|c| c.is_ascii_hexdigit()).collect();
    let mut bytes = Vec::new();
    for i in (0..hex_chars.len()).step_by(2) {
        let chunk = &hex_chars[i..std::cmp::min(i+2, hex_chars.len())];
        if let Ok(b) = u8::from_str_radix(chunk, 16) {
            bytes.push(b);
        }
    }
    bytes
}

// Helper to print hex
fn print_hex_be(label: &str, bytes: &[u8]) {
    print!("{}", label);
    for b in bytes {
        print!("{:02X} ", b);
    }
    println!();
}

fn print_hex_plain(label: &str, bytes: &[u8]) {
    print!("{}", label);
    let mut s = String::new();
    for b in bytes {
        s.push_str(&format!("{:02X} ", b));
    }
    println!("{}", s.trim());
}

fn generate_k(q: &BigInt) -> BigInt {
    // Generate 0 < k < q using rand
    let bit_len = q.bit_len();
    let mut rng = OsRng;
    
    loop {
        // Generate random bytes enough to cover bit_len
        let byte_len = (bit_len + 7) / 8;
        let mut bytes = vec![0u8; byte_len];
        rng.fill_bytes(&mut bytes);
        
        // Mask excess bits
        if bit_len % 8 != 0 {
            bytes[0] &= 0xFF >> (8 - (bit_len % 8));
        }

        let k = BigInt::from_be_bytes(&bytes);
        
        if k > BigInt::from_u64(0) && k < *q {
            return k;
        }
    }
}

fn main() {
    // 1. Read all Stdin
    let mut buffer = String::new();
    io::stdin().read_to_string(&mut buffer).expect("Failed to read stdin");
    
    // 2. Parse Hex Stream
    let data = parse_hex_input(&buffer);
    
    // 3. Define Offsets
    // p: 256 bytes
    // q: 32 bytes
    // g: 256 bytes
    // x: 32 bytes
    // len: 2 bytes (Little Endian!)
    // msg: len bytes
    
    let mut cursor = 0;
    
    let p_bytes = &data[cursor..cursor+256]; cursor += 256;
    let q_bytes = &data[cursor..cursor+32]; cursor += 32;
    let g_bytes = &data[cursor..cursor+256]; cursor += 256;
    let x_bytes = &data[cursor..cursor+32]; cursor += 32;
    
    // Length is Little Endian u16
    let len_low = data[cursor];
    let len_high = data[cursor+1];
    cursor += 2;
    let msg_len = ((len_high as u16) << 8) | (len_low as u16);
    
    let msg_bytes = &data[cursor..cursor+(msg_len as usize)];
    
    // 4. Convert to BigInts
    let p = BigInt::from_be_bytes(p_bytes);
    let q = BigInt::from_be_bytes(q_bytes);
    let g = BigInt::from_be_bytes(g_bytes);
    let x = BigInt::from_be_bytes(x_bytes);
    
    // 5. Hash Message
    let mut hasher = Sha256::new();
    hasher.update(msg_bytes);
    let hash_out = hasher.finalize();
    
    // Convert hash to integer (H(m))
    // Standard DSA: take leftmost N bits of hash if hash len > N.
    // Here both are 256 bits.
    let z = BigInt::from_be_bytes(&hash_out);
    
    // 6. Sign Loop
    let mut r: BigInt;
    let mut s: BigInt;
    
    loop {
        // Step 1: k random, 0 < k < q
        let k = generate_k(&q);
        
        // Step 2: r = (g^k mod p) mod q
        let gk_p = g.mod_pow(&k, &p);
        r = gk_p.div_rem(&q).1;
        
        if r.is_zero() { continue; }
        
        // Step 3: s = k^-1 * (z + x*r) mod q
        let k_inv = k.mod_inverse(&q);
        
        let xr = x.mul(&r);
        let z_xr = z.add(&xr); // z + xr
        
        // Reduce z_xr mod q before multiply to keep sizes check
        let z_xr_mod_q = z_xr.div_rem(&q).1;
        
        let s_pre = k_inv.mul(&z_xr_mod_q);
        s = s_pre.div_rem(&q).1;
        
        if s.is_zero() { continue; }
        
        break;
    }
    
    // 7. Output
    print_hex_plain("1 ", &r.to_be_bytes_padded(32));
    print_hex_plain("2 ", &s.to_be_bytes_padded(32));
}