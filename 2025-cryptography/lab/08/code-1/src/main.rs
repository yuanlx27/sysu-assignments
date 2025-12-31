use std::io;
use std::io::{Read, Write};

fn read_u64_le<R: Read>(reader: &mut R) -> io::Result<u64> {
    let mut buf = [0u8; 8];
    reader.read_exact(&mut buf)?;
    Ok(u64::from_le_bytes(buf))
}

fn read_u32_le<R: Read>(reader: &mut R) -> io::Result<u32> {
    let mut buf = [0u8; 4];
    reader.read_exact(&mut buf)?;
    Ok(u32::from_le_bytes(buf))
}

fn pow(a: u64, mut b: u64, p: u64) -> u64 {
    let mut a = a as u128;
    let mut c = 1u128;
    let p = p as u128;
    while b > 0 {
        if b % 2 == 1 {
            c = c * a % p;
        }
        a = a * a % p;
        b /= 2;
    }
    c as u64
}

fn step(g: u64, h: u64, p: u64, q: u64, r: u64, x: u64) -> u64 {
    let e = pow(h, x, q);
    let y = pow(g, e, p);
    y % r
}

fn main() -> io::Result<()> {
    let stdin = io::stdin();
    let mut handle = stdin.lock();

    let r = read_u64_le(&mut handle)?;
    let q = r * 2 + 1;
    let p = q * 2 + 1;

    let g = read_u64_le(&mut handle)?;
    let h = read_u64_le(&mut handle)?;
    let mut x = read_u64_le(&mut handle)?;

    let l = read_u32_le(&mut handle)? as usize;
    let mut bits = vec![0u8; l * 8];
    for i in 0..l {
        for j in 0..8 {
            x = step(g, h, p, q, r, x);
            bits[i * 8 + j] = (x % 2) as u8;
        }
    }

    let mut ones_count = 0u32;
    let mut ones_count_per_block = vec![0u8; l / 8];
    for i in 0..bits.len() {
        if bits[i] == 1 {
            ones_count += 1;
            if i / 64 < l / 8 {
                ones_count_per_block[i / 64] += 1;
            }
        }
    }

    let mut runs_count = 0u32;
    let mut runs_distribution = [0u32; 8];

    let mut cur_val = bits[0];
    let mut cur_len = 1;
    for &bit in &bits[1..] {
        if bit == cur_val {
            cur_len += 1;
        } else {
            runs_count += 1;
            if cur_len <= 8 {
                runs_distribution[cur_len - 1] += 1;
            }
            cur_val = bit;
            cur_len = 1;
        }
    }
    runs_count += 1;
    if cur_len <= 8 {
        runs_distribution[cur_len - 1] += 1;
    }

    let random_data = bits
        .chunks(8)
        .map(|chunk| chunk.iter().rev().fold(0u8, |acc, &bit| (acc << 1) | bit))
        .collect::<Vec<u8>>();

    let stdout = io::stdout();
    let mut handle = stdout.lock();

    handle.write_all(&random_data)?;
    handle.write_all(&ones_count.to_le_bytes())?;
    handle.write_all(&ones_count_per_block)?;
    handle.write_all(&runs_count.to_le_bytes())?;
    for &count in &runs_distribution {
        handle.write_all(&count.to_le_bytes())?;
    }

    Ok(())
}
