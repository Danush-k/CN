# Student Record Query System (Socket Programming)

This folder contains TCP and UDP client-server implementations to query student details from a CSV database (`students.csv`).

## Files

- [students.csv](file:///Users/danush/Desktop/SEM-5/CN-LAB/student_info_sockets/students.csv): Student database containing Roll No, Name, Department, Semester, and CGPA.
- [tcp_server.c](file:///Users/danush/Desktop/SEM-5/CN-LAB/student_info_sockets/tcp_server.c): Concurrent TCP server (using POSIX threads) listening on port `8080`.
- [tcp_client.c](file:///Users/danush/Desktop/SEM-5/CN-LAB/student_info_sockets/tcp_client.c): TCP client to query student details by roll number.
- [udp_server.c](file:///Users/danush/Desktop/SEM-5/CN-LAB/student_info_sockets/udp_server.c): UDP server listening on port `8081`.
- [udp_client.c](file:///Users/danush/Desktop/SEM-5/CN-LAB/student_info_sockets/udp_client.c): UDP client to query student name by roll number.

---

## How to Compile & Run

### 1. TCP Server & Client (Port 8080)

**Terminal 1 (Server):**
```bash
gcc -Wall -Wextra -pthread tcp_server.c -o tcp_server
./tcp_server
```

**Terminal 2 (Client):**
```bash
gcc -Wall -Wextra tcp_client.c -o tcp_client
./tcp_client
```

---

### 2. UDP Server & Client (Port 8081)

**Terminal 1 (Server):**
```bash
gcc -Wall -Wextra udp_server.c -o udp_server
./udp_server
```

**Terminal 2 (Client):**
```bash
gcc -Wall -Wextra udp_client.c -o udp_client
./udp_client
```
