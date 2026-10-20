# Error Detection Lab (Checksum & CRC)

Simple programs in C using TCP sockets simulating reliable data transmission using **Checksum** and **Cyclic Redundancy Check (CRC)**.

---

## Easiest Way to Run (1 Terminal, 1 Command)

You can run each complete experiment directly in a single terminal. It performs the sender calculations, transmits over a real TCP socket, and displays receiver verification automatically!

### 1. Checksum:
```bash
cd /Users/danush/Desktop/SEM-5/CN-LAB/error_detection
./checksum
```
- Enter string: e.g. `hello`
- Enter `0` for normal run (or `1` to inject error)

### 2. CRC:
```bash
cd /Users/danush/Desktop/SEM-5/CN-LAB/error_detection
./crc
```
- Enter binary: e.g. `100100` (or text `A`)
- Enter `0` for normal run (or `1` to inject bit error)

---

## Alternative: Using 2 Separate Terminals

If you prefer two separate terminal windows:

### Checksum:
- **Terminal 1 (Receiver):** `./checksum_receiver`
- **Terminal 2 (Sender):** `./checksum_sender`

### CRC:
- **Terminal 1 (Receiver):** `./crc_receiver`
- **Terminal 2 (Sender):** `./crc_sender`

*(Note: Always start the Receiver before running the Sender)*

---

## How to Recompile
```bash
make
```
