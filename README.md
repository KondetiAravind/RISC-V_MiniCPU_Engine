# 🔧 RISC-V MiniCPU Engine

A lightweight educational **RISC-V processor simulator** that includes an assembler, instruction parser, caching, and a pipelined CPU model. This project converts high-level assembly-like input into binary, simulates its execution, and mimics core behaviors of a simplified CPU architecture.

---

## 📌 Features

- 🧠 Custom instruction parsing and preprocessing
- ⚙️ Assembler that converts RISC-V-like instructions to binary
- 🧮 Execution engine that mimics CPU behavior (registers, memory, branching, etc.)
- 🧰 Simple caching mechanism (optional module)
- 🔄 Supports control flow (`beq`, `blt`), arithmetic (`add`, `sub`, `div`, `mod`, `mul`), and memory operations (`lw`, `sw`)
- 🚀 Modular pipelining design for instruction processing

---

## 📁 Project Structure

```plaintext
.
├── main.cpp              # Entry point; handles input and control flow
├── cpu.cpp               # CPU class implementation (registers, instruction execution)
├── addressing.h          # Assembler/parser to convert to binary instruction format
├── sample_inputs.txt     # Sample inputs for testing
├── README.md             # You're here

## 🚀 Getting Started
### 📥 Prerequisites
C++17 compiler (GCC or Clang recommended)

Linux/macOS/Windows (terminal-based)

g++ installed
