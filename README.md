# 🔧 RISC-V MiniCPU Engine

**RISC-V MiniCPU Engine** is a lightweight educational processor simulator that emulates core functionalities of a RISC-V-like architecture. It includes a custom assembler, instruction parser, a pipelined CPU model, and optional cache support. The simulator reads user-defined assembly-style instructions, converts them to binary, and simulates execution with control flow, arithmetic operations, and memory handling.

---

## 📌 Features

- ✅ Custom instruction parsing and tokenization  
- ✅ Converts assembly to binary (assembler logic)  
- ✅ Simulates register and memory state changes  
- ✅ Handles control flow instructions like `beq`, `blt`, etc.  
- ✅ Includes memory operations (`lw`, `sw`)  
- ✅ Extendable pipelined CPU structure  
- ✅ Organized modular structure (`parsing`, `Tagging`, `CPU`)  

---

## 📁 Project Structure

```
.
├── main.cpp              # Entry point and input controller
├── cpu.cpp               # Simulated CPU implementation
├── addressing.h          # Assembler for converting instructions to binary
├── README.md             # Project documentation
```

---

## 🚀 Getting Started

### 📥 Prerequisites

- C++17 or later compiler (GCC/Clang)
- Any OS (Linux, Windows, macOS)
- g++ installed for compilation

### ⚙️ Compilation

```bash
g++ main.cpp -o RISC-V-MiniCPU
```

Make sure `cpu.cpp` and `addressing.h` are in the same directory as `main.cpp`.

### ▶️ Running the Program

```bash
./RISC-V-MiniCPU
```

You’ll be prompted to provide input (e.g., value for `n`), and the selected instruction set will be assembled, displayed as binary, and simulated by the CPU engine.

---

## 🧪 Sample Instruction Input

```assembly
ADDI x1,x0,5
ADDI x2,x0,0
ADDI x3,x0,1
ADD x2,x2,x3
SUB x4,x3,x1
ADDI x3,x3,1
BEQ x4,x0,2
BEQ x0,x0,-4
```

### 🔄 Output Flow

1. Parsed to tokens
2. Assembled into binary
3. Passed to CPU simulator
4. Registers and memory updated step-by-step

---

## 📚 Concepts Demonstrated

- Instruction parsing and tokenization  
- Assembly-level binary generation  
- Basic pipelined CPU simulation  
- Use of C++ classes and modular design  
- Control and data flow management  

---

## 🛠 Future Enhancements

- [ ] Add file input/output support  
- [ ] Extend instruction set coverage  
- [ ] Simulate hazards and data forwarding  
- [ ] Implement pipeline stage visualization  
- [ ] Add memory hierarchy (cache/L1 simulation)

---

## 👨‍💻 Author

**Kondeti Aravind**  
School of Electrical and Computer Sciences (SECS), IIT Bhubaneswar  
📧 22cs02008@iitbbs.ac.in  
🔗 [GitHub Profile](https://github.com/kondetiaravind)

---

## 📄 License

This project is developed for academic and learning purposes. You are free to use and modify it under the MIT License.
