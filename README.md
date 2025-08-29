# Altitude Data Simulation & Validation

## 📌 Project Overview
This project simulates altitude data for an aircraft/vehicle and validates it using client-server communication.  
1. **Server** – Generates simulated altitude data with smooth increments and random turbulence, streams it to a file (`altitude.csv`), and sends it to the client over a socket.
---

## 🗂️ Files in This Project
- `server.c` → Program to generate altitude data, log it into CSV, and send it over TCP.
- `altitude.csv` → Sample generated altitude dataset.
- `README.md` → Documentation (this file).

---

## ⚙️ Requirements
- **Operating System:** Linux/Windows with GCC installed.
- **Compiler:** GCC (for compiling C programs).
- **Tools/Libraries:** 
  - Standard C libraries (`stdio.h`, `stdlib.h`, `time.h`, `string.h`, `unistd.h`, `arpa/inet.h`, `sys/socket.h`).

---

## 🚀 Compilation & Execution

### Step 1: Compile Programs
```bash
gcc server.c -o server
