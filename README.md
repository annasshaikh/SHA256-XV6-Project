
# SHA-256 Implementation in XV6

## Description

This project presents the implementation of the SHA-256 cryptographic hash algorithm in both user space and kernel space within the XV6 operating system. It includes the creation of a system call for efficient user-kernel interaction. The user space implementation emphasizes modularity for application-level use, while the kernel-space integration ensures secure and efficient processing of sensitive data. The system call bridges these implementations, enabling user space programs to utilize kernel-level functionality. This work also highlights and addresses limitations such as XV6 page limitations and includes utilities for unit testing and runtime analysis. [1 (Abstract)]

## Features

*   **SHA-256 Algorithm:** Implements the full SHA-256 hashing process, including padding, message scheduling, and 64 rounds of computation. [1 (I, II, A, B, C, E), 2 (E)]
*   **User Space Implementation (`sha256user`):**
    *   Provides a command-line utility to compute SHA-256 hashes for strings and files. [3 (III, Usage Documentation)]
    *   Example: `sha256user string "hello world"` [3 (Usage Documentation)]
    *   Example: `sha256user file <file_path>` [3 (Usage Documentation)]
*   **Kernel Space Implementation:**
    *   Integrates SHA-256 hashing functionality directly within the XV6 kernel. [3 (IV)]
    *   Includes a test function `sha256test()` invoked during kernel startup to hash a predefined string ("XV6 Loading") and print the result. [3 (1- Hashing the startup sequence), 4 (The Output of Startup Sequence.)]
*   **System Call for SHA-256 (`sys_sha256call`):**
    *   A system call allowing user-space programs to request the kernel to perform SHA-256 hashing. [4 (2- System Call)]
    *   Handles argument passing (input string, length, output buffer address) and memory management (kalloc, copyin, copyout). [4, 5 (Sha256call() System Call)]
*   **Handling Large Inputs (Streaming Hash / Chunking):**
    *   Addresses the 4KB page limitation in XV6 when handling large inputs for hashing. [5 (V. THE 4 KB PAGE LIMITATION)]
    *   User space implementation (`sha256hash` function) reads input in chunks (e.g., 3000 bytes) and uses `sha256_append` to process large data incrementally. [6 (VI. EXPLOITING THE SHA256 STREAMING HASH)]
    *   Introduces three system calls for streaming hash operations in the kernel:
        *   `sys_sha256initcall()`: Initializes the SHA-256 context in the kernel. [6, 7]
        *   `sys_sha256appendcall()`: Appends a chunk of data to the ongoing SHA-256 hash calculation in the kernel. [7]
        *   `sys_sha256finalizecall()`: Finalizes the SHA-256 calculation and returns the hash. [7]
*   **Password Protection Feature:**
    *   Demonstrates SHA-256 utility by adding a password prompt to the XV6 shell (`sh.c`). [7 (VIII. THE PASSWORD FEATURE)]
    *   User input is hashed and compared against a predefined hardcoded hash for validation. [7]
*   **Runtime Utility (`runtime`):**
    *   A user-space program to measure the execution time (in system clock ticks) of other programs. [8 (IX. THE RUNTIME UTITY)]
    *   Uses a `sys_getr_time` system call which reads the machine-mode cycle counter (`r_time()`). [8]
    *   Example: `runtime sha256user file README` [8 (User space Implementation test.)]
    *   Used to compare the performance of user space vs. system call implementations of SHA-256. [8]

## How SHA-256 is Implemented

The SHA-256 algorithm involves the following key steps:

1.  **Padding Bits:** The original message is padded to ensure its length is 64 bits less than a multiple of 512 bits. This involves appending a '1' bit, followed by '0's. [1 (A. Padding Bits)]
2.  **Appending Length Bits:** A 64-bit representation of the original message length is appended to the padded message. The entire message is now an exact multiple of 512 bits. [1 (B. Padding Bits)]
3.  **Initializing Hash Buffers and Round Constants:** Eight initial hash values (A-H) and 64 round constants (k[0..63]) are used. [1 (C. Initialize Round Constants), 2]
4.  **Processing Message in 512-bit Chunks:**
    *   Each chunk undergoes 64 rounds of operations. [1 (II), 2 (D)]
    *   A message schedule array (w) is prepared from the input data. For the first 16 words, it's derived directly from the input buffer. For subsequent words, values are computed based on prior entries using bitwise rotations, shifts, and modular additions (`update_w` function). [2 (update_w function), 3 (Case 1, Case 2)]
    *   The `sha256_block` function processes each 512-bit block, combining the message schedule (w), predefined constants (k), and the current hash state through 64 rounds, updating the hash state. [2 (E. Computation of Hash)]
5.  **Output:** The final 256-bit hash value is produced. [1 (I)]

## Addressing XV6 Limitations

*   **4KB Page Limitation:** XV6's `kalloc()` allocates memory in 4KB pages. `argstr()` can only copy strings up to this size. [5 (V. THE 4 KB PAGE LIMITATION)]
*   **Solutions for Large Strings:**
    *   **Streaming/Chunking:** The input data is processed in smaller, manageable chunks (e.g., 3000KB in the user-space file hashing implementation). This avoids needing to load the entire large file/string into memory at once. [6 (VI. EXPLOITING THE SHA256 STREAMING HASH)]
    *   **Multiple System Calls for Streaming:** For kernel-side hashing of large data, three distinct system calls (`sha256initcall`, `sha256appendcall`, `sha256finalizecall`) are implemented to manage the state of the hash computation across multiple data chunks. [6 (VII), 7]

## Usage Examples

### User Space Hashing

*   **String Input:**
    ```bash
    $ sha256user string "hello world"
    The SHA-256 sum of "hello world" is:
    b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9
    ```
    [3 (Usage Documentation)]

*   **File Input:**
    ```bash
    $ sha256user file README
    Processing input as a file:
    SHA-256 hash of README: <hash_value>
    ```
    [3 (Usage Documentation)]

### Kernel Startup SHA-256 Test

During XV6 boot:
```
xv6 kernel is booting
The SHA-256 sum of "XV6 Loading" is: 19bfa14772d0ebca1c3607a71b440464452b3bb980c83be7c1bfe90d29d0df
...
```
[3, 4 (The Output of Startup Sequence.)]

### Runtime Measurement

```bash
$ runtime sha256user file README
SHA-256 hash of README: <hash_value>
Execution time for sha256user: <time_in_ms> ms
```
[8 (User space Implementation test.)]

```bash
$ runtime sha256test README
SHA-256 hash of README: <hash_value>
Execution time for sha256test: <time_in_ms> ms
```
[8 (Syscall Implementation Tests.)]

### Shell Password Prompt

Upon starting the shell:
```
init: starting sh
Password: $
```
[7 (Sample Output of the SHA 256 Login Validator)]

*(Note: Actual hash values and execution times will vary. Examples are illustrative based on the provided paper.)*

## Project Structure (Inferred from Paper)

*   `user/sha256user.c`: User-space SHA-256 implementation and command-line tool. [3]
*   `user/runtime.c`: Runtime measurement utility. [8]
*   `kernel/sysproc.c`: Likely contains implementations for system calls like `sys_sha256call`, `sys_getr_time`, and the streaming hash calls. [5, 8]
*   `kernel/main.c` or similar: Modified to include `sha256test()` in the boot sequence. [3]
*   `kernel/defs.h` or similar: System call definitions.
*   `kernel/sha256.c` (or similar): Core SHA-256 logic for kernel use.
*   `sh.c`: Modified to include the password feature. [7]
*   Makefile: Modified to include `sha256user`, `sha256test`, `sha256pipe`, `runtime` in `UPROGS`. [3]

*(This README was generated based on the content of the research paper "SHA 256 Implementation In XV6" by Muhammad Annas Shaikh and Dr Salman Zaffar, IBA Karachi.)*
```
