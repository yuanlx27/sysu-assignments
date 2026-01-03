# Experiment 7-1: Implementing DSA Signatures

## Time & Environment
- **Compile time limit:** 15000 ms  
- **Runtime time limit:** 5000 ms  
- **Compile memory limit:** 768 MB  
- **Runtime memory limit:** 32 MB  
- **CPU:** 1 vCPU  

---

## Experiment Description

This experiment requires implementing the **DSA (Digital Signature Algorithm)**:
- **Signature generation** (Experiment 7-1)
- **Signature verification** (Experiment 7-2)

The algorithm follows the description in *Modern Cryptography*, Section 7.4 (page 232).

---

## Algorithm Specification (DSA)

### Parameter Selection
1. Choose parameter sizes **(L, N)** and a hash function **H**, where:
   - $N \le$ output length of $H$
2. Early NIST standard **FIPS 186-1** recommended:
   - (L, N) = (1024, 160) with SHA-1
3. **FIPS 186-4 (Section 4.2)** recommends:
   - (1024,160), (2048,224), (2048,256), (3072,256)
4. **FIPS 186-5 (Feb 2023)**:
   - DSA is **no longer recommended** for generating new signatures

> In this experiment, DSA is still required for educational purposes.

---

### Key Generation
1. Choose an **L-bit prime** $p$
2. Choose an **N-bit prime** $q$, such that $q \mid (p - 1)$
3. Choose generator $g$ of order $q$ modulo $p$
    - Select random $1 < h < p$
    - Compute
        $$
            g = h^{(p-1)/q} \bmod p
        $$
4. Choose private key $x$, where $0 < x < q$
5. Compute public key:
    $$
        y = g^x \bmod p
    $$

---

### Signature Generation
To sign message $m$:

1. Choose random nonce $k$, where $0 < k < q$
2. Compute:
    $$
        r = (g^k \bmod p) \bmod q
    $$
3. Compute:
    $$
        s = k^{-1}(H(m) + xr) \bmod q
    $$
4. If $r = 0$ or $s = 0$, choose a new $k$ and retry
5. Output signature **(r, s)**

---

### Signature Verification
Given message $m$, signature $(r, s)$, and public key $y$:

1. Compute:
    $$
        w = s^{-1} \bmod q
    $$
2. Compute:
    $$
        u_1 = H(m) \cdot w \bmod q \\
        u_2 = r \cdot w \bmod q
    $$
3. Verify:
    $$
        (g^{u_1} \cdot y^{u_2} \bmod p) \bmod q = r
    $$

---

## Fixed Parameters for This Experiment

- **(L, N) = (2048, 256)**
- **Hash function:** SHA-256
- All large integers are interpreted as **big-endian**, unless explicitly stated otherwise.

---

## Input Format

The input consists of the following fields, in order:

1. `uint8_t[256]` — **p**, 2048-bit prime, **big-endian**
2. `uint8_t[32]` — **q**, 256-bit prime, **big-endian**
3. `uint8_t[256]` — **g**, generator, **big-endian**
4. `uint8_t[32]` — **x**, private key, **big-endian**
5. `uint16_t` — **message length**, **little-endian**
6. `uint8_t[]` — **message bytes**, raw byte sequence

---

## Output Format

- Output the DSA signature **(r, s)**:
  - `r`: 32 bytes, **big-endian**
  - `s`: 32 bytes, **big-endian**

---

## Randomness Requirement

- Use a **cryptographically secure random number generator** to generate the nonce $k$

---

## Judging Notes

- DSA signatures are **not unique** due to randomness in $k$
- The judge uses a **Special Judge**
- The judge:
  - Recomputes the public key from input $x$
  - Verifies the submitted signature against the given message

> Any valid signature will be accepted.

---

## Sample Data

- Sample inputs and outputs are provided
- The sample output is **only one possible valid result**
- The nonce $k$ used in the sample is disclosed for debugging purposes
- You may temporarily hardcode this $k$ to test your implementation

---

## Implementation Notes

- Convert all big-endian byte arrays to big integers before computation
- Read the message length as **little-endian**
- Convert final values $r$ and $s$ back to **32-byte big-endian** arrays before output
