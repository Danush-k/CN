# HTTP & HTTPS Socket Lab

Simple guide to run the programs directly from your terminal.

---

## 1. Mandatory Program: HTTP Client (`http_client.c`)
> **One-line Description:** Connects to a Python HTTP server via TCP socket, downloads resources (HTML, text, PDF, image), and prints Response Time, Download Time, Data Size, and Throughput.

### How to Run:

**Terminal 1 (Start Python Web Server):**
```bash
cd CN_Web_Server
python3 -m http.server 8000
```

**Terminal 2 (Run C HTTP Client):**
```bash
./http_client
```
Enter any URL when prompted:
- `http://localhost:8000/sample.pdf`
- `http://localhost:8000/image.jpg`
- `http://localhost:8000/index.html`
- `http://localhost:8000/notes.txt`

---

## 2. Advanced Program: Secure HTTPS Client (`https_client.c`)
> **One-line Description:** Connects to secure HTTPS websites (Port 443) using OpenSSL, performs SSL/TLS handshake, decrypts webpage content, and prints TCP Connection Time, SSL Handshake Time, RTT, and Data Transfer Rate.

### How to Run:

**Terminal:**
```bash
./https_client
```
Enter URL when prompted:
- `https://example.com/`

---

## How to Recompile (If you modify code)
```bash
make
```
