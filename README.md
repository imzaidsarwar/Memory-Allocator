# Custom Memory Allocator in C

A from-scratch implementation of a dynamic memory allocator in C that mimics the behavior of `malloc`, `free`, `calloc`, and `realloc`.  
This project explores low-level memory management concepts such as heap control, fragmentation handling, block splitting, and alignment.

---

## 🚀 Features

- Custom implementation of:
  - `my_malloc`
  - `my_free`
  - `my_calloc`
  - `my_realloc`
- Manual heap management using `sbrk` / `brk`
- Block metadata management using headers
- Memory alignment handling
- Block splitting to reduce internal fragmentation
- Block coalescing to reduce external fragmentation
- Thread-unsafe by design (single-threaded allocator)

---

## 🧠 Concepts Covered

- Heap memory layout
- Internal vs external fragmentation
- Free list management
- First-fit allocation strategy
- Metadata overhead trade-offs
- Alignment and padding
- Low-level systems programming in C

---

## 🏗️ Project Structure

```text

Memory-Allocator/
├── include/
│   └── allocator.h        # Public API + data structures
│
├── src/
│   ├── allocator.c        # malloc/free/calloc/realloc implementation
│   └── main.c             # Test / driver program
│
├── README.md
