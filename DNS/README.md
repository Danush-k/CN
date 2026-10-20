# UDP DNS Simulation Lab

Simple client-server program in C using UDP to simulate a DNS server and DNS client with local caching, timeout handling, and NXDOMAIN error detection.

---

## Files in this Folder

- [dns_server.c](file:///Users/danush/Desktop/SEM-5/CN-LAB/DNS/dns_server.c): UDP DNS Server with domain-to-IP database.
- [dns_client.c](file:///Users/danush/Desktop/SEM-5/CN-LAB/DNS/dns_client.c): UDP DNS Client with client-side caching & timeout error handling.
- [Makefile](file:///Users/danush/Desktop/SEM-5/CN-LAB/DNS/Makefile): Build automation script.

---

## How to Compile

In your terminal, navigate to the `DNS` directory and run:
```bash
cd /Users/danush/Desktop/SEM-5/CN-LAB/DNS
make
```

---

## How to Run

### Step 1: Start DNS Server (Terminal 1)
```bash
./dns_server
```
> Server starts listening on UDP port `9053`.

---

### Step 2: Run DNS Client (Terminal 2)
```bash
./dns_client
```

---

## Test Inputs & What They Demonstrate

| Enter Domain Name | What it Demonstrates | Expected Output |
| :--- | :--- | :--- |
| **`google.com`** *(1st time)* | Normal DNS lookup over UDP | `[SUCCESS] google.com -> 142.250.190.46 (Cached: Yes)` |
| **`google.com`** *(2nd time)* | **Client Caching (Cache Hit)** | `[CACHE HIT] Resolved from local cache (Latency: 0.00 ms)` |
| **`notfound.com`** | **NXDOMAIN (Non-existent Domain)** | `[ERROR] NXDOMAIN: Domain not found in DNS database.` |
| **`invaliddomain`** *(no dot)* | **Invalid Format Error** | `[ERROR] Invalid domain format! Example: 'google.com'` |
| **`timeout.com`** | **Timeout Simulation** | `[ERROR] Request Timed Out! DNS Server did not respond within 2.0s.` |
| **`analysis`** | **UDP Suitability Report** | Displays Overhead, Latency, and Reliability comparison. |
| **`exit`** | Exit client | Closes connection. |

---

## UDP Suitability for DNS Services

1. **Communication Overhead:**
   - UDP header is only **8 bytes** (compared to TCP's 20–60 bytes).
   - No connection setup (SYN, SYN-ACK, ACK) and no teardown overhead.
2. **Latency:**
   - Ultra-low latency (**1 Round Trip Time**). A single query datagram and a single response datagram.
3. **Reliability:**
   - Because DNS queries are idempotent (repeating a query has no side effects), reliability is easily handled at the **application layer** using client-side timeouts and retransmission. If a packet is lost, the client simply retries.
