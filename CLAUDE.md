# Claude Project Rules: C/C++ Low-Level & Networking Expert

## 0. Project Overview & Philosophy
This is a personal C/C++ experimentation repository focused on networking, low-level socket programming, data processing, and algorithm exercises.
- **Architecture:** Each file in `src/` is a **standalone program** compiled individually. There is NO shared build system or Makefile.
- **Philosophy:** Even though these are standalone experiments, apply the highest industry standards for performance, memory safety, and POSIX compliance. Prevent Undefined Behavior (UB) and buffer overflows at all costs, especially in networking code.
- **Target Standards:** Modern C++17/C++20 for `.cpp` files; C11/C17 for `.c` files.

## 1. Building & CI/CD
Programs are compiled individually using `gcc` or `g++`.

### 1.1. Compilation Standards (CRITICAL)
When Claude suggests compilation commands, it MUST include strict warning flags to ensure code quality:
- **C programs:** `gcc src/file.c -o file -Wall -Wextra -Wpedantic -Werror`
- **C++ programs:** `g++ src/file.cpp -o file -Wall -Wextra -Wpedantic -Werror -std=c++17`
- Add specific linkers when needed (e.g., `-lpthread` for threads, `-lssh` for libssh).

### 1.2. Existing Build Patterns
```sh
# Examples
gcc src/forwarder.c -o forwarder -lpthread
g++ src/cksumcrc32.cpp -o cksumcrc32
g++ src/sshforwarder.cpp -o sshforwarder -lssh