# TCP and UDP Traffic Analysis Over Bottleneck Link in NS-3

Simulates simultaneous TCP and UDP packet transmission between four end nodes across two intermediate routers connected via a bottleneck link with DropTail queueing.

---

## 1. Network Topology

```text
Node 0 (TCP Source) ──── (10 Mbps, 2 ms) ────┐               ┌──── (10 Mbps, 2 ms) ──── Node 3 (TCP Sink)
                                             ▼               │
                                         [Router 1] ─── Bottleneck Link ─── [Router 2]
                                             ▲      (2 Mbps, 10 ms, 50p DropTail) │
Node 1 (UDP Source) ──── (10 Mbps, 2 ms) ────┘               └──── (10 Mbps, 2 ms) ──── Node 2 (UDP Sink)
```

- **Access Links**: 10 Mbps, 2 ms delay
- **Bottleneck Link (R1 ↔ R2)**: 2 Mbps, 10 ms delay, DropTail queue (50 packets)
- **Flow 1 (TCP)**: Node 0 $\rightarrow$ Node 3 (Port 5000, 1024-byte packets, BulkSend)
- **Flow 2 (UDP)**: Node 1 $\rightarrow$ Node 2 (Port 6000, 512-byte packets, 1 Mbps OnOff)
- **Simulation Duration**: 20.0 seconds

---

## 2. Files in this Directory

- [tcp_udp_traffic.cc](file:///Users/danush/Desktop/SEM-5/CN-LAB/TCP%20UDP%20Traffic%20using%20NS3/tcp_udp_traffic.cc): Complete NS-3 simulation code with FlowMonitor metric computation.
- [run.sh](file:///Users/danush/Desktop/SEM-5/CN-LAB/TCP%20UDP%20Traffic%20using%20NS3/run.sh): Runner script for Experiments 1, 2, 3, and 4.
- `tcp-udp-bottleneck-*.pcap`: Wireshark packet captures on the bottleneck link.
- `tcp-udp-access-*.pcap`: Wireshark packet captures on the access links.

---

## 3. How to Run

Navigate into this directory:
```bash
cd "/Users/danush/Desktop/SEM-5/CN-LAB/TCP UDP Traffic using NS3"
```

### Run Default Simulation (2 Mbps Bottleneck, 10 ms Delay, 1 Mbps UDP, 50-pkt Queue):
```bash
./run.sh
```

### Run Individual Experiments:
```bash
./run.sh exp1    # Experiment 1: Bottleneck Bandwidth (2, 5, 10 Mbps)
./run.sh exp2    # Experiment 2: UDP Traffic Rate (0.5, 1, 2, 5 Mbps)
./run.sh exp3    # Experiment 3: Queue Size (20, 50, 100 packets)
./run.sh exp4    # Experiment 4: Propagation Delay (5, 10, 20, 50 ms)
```

### Run All Experiments in One Go:
```bash
./run.sh all
```

---

## 4. Observation Tables

### Experiment 1: Bottleneck Bandwidth Variation

| Bottleneck Bandwidth | Protocol | Throughput | Packet Loss (%) | Average Delay (ms) | TCP Retransmissions (Rate %) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **2 Mbps** | **TCP** | **0.876 Mbps** | 0.382 % | 134.25 ms | 29 (0.744 %) |
| | **UDP** | **0.966 Mbps** | 2.652 % | 134.20 ms | — |
| **5 Mbps** | **TCP** | **3.560 Mbps** | 0.145 % | 59.46 ms | 45 (0.284 %) |
| | **UDP** | **0.997 Mbps** | 0.000 % | 55.52 ms | — |
| **10 Mbps** | **TCP** | **8.075 Mbps** | 0.022 % | 37.76 ms | 85 (0.237 %) |
| | **UDP** | **0.998 Mbps** | 0.000 % | 35.49 ms | — |

> **Key Observation**: When bottleneck bandwidth increases, TCP throughput scales up to consume the available capacity (from 0.88 Mbps to 8.08 Mbps), while delay drops drastically from 134 ms to 37 ms. UDP consistently gets its requested ~1 Mbps.

---

### Experiment 2: UDP Traffic Rate Variation (Bottleneck = 2 Mbps)

| UDP Offered Rate | UDP Throughput | UDP Loss (%) | TCP Throughput | TCP Loss (%) | UDP Delay (ms) | TCP Delay (ms) |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **0.5 Mbps** | 0.497 Mbps | 0.000 % | **1.336 Mbps** | 0.401 % | 123.88 ms | 129.15 ms |
| **1.0 Mbps** | 0.966 Mbps | 2.652 % | **0.876 Mbps** | 0.382 % | 134.20 ms | 134.25 ms |
| **2.0 Mbps** | 0.997 Mbps | 48.345 % | **0.856 Mbps** | 0.468 % | 752.15 ms | 138.97 ms |
| **5.0 Mbps** | 1.011 Mbps | 38.947 % | **0.843 Mbps** | 0.291 % | 5102.15 ms | 138.13 ms |

> **Key Observation**: UDP is unconstrained by congestion feedback. When UDP sends at 2 or 5 Mbps into a 2 Mbps bottleneck, it floods the router buffer, causing massive packet loss (~40-48%) and bufferbloat delay. TCP backs off gracefully, maintaining its fair share around 0.85 Mbps.

---

### Experiment 3: Queue Size Variation (20, 50, 100 packets)

| Queue Size | Protocol | Throughput | Packet Loss (%) | Average Delay (ms) | TCP Retransmissions |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **20 pkts** | **TCP** | 0.889 Mbps | 1.992 % | **80.95 ms** | 15 |
| | **UDP** | 0.955 Mbps | 4.140 % | **77.49 ms** | — |
| **50 pkts** | **TCP** | 0.876 Mbps | 0.382 % | **134.25 ms** | 29 |
| | **UDP** | 0.966 Mbps | 2.652 % | **134.20 ms** | — |
| **100 pkts** | **TCP** | 0.863 Mbps | 0.412 % | **213.23 ms** | 45 |
| | **UDP** | 0.980 Mbps | 0.798 % | **212.41 ms** | — |

> **Key Observation**: Smaller queue (20 pkts) yields **lower delay (80 ms)** but **higher packet loss**. Larger queue (100 pkts) absorbs bursts, lowering packet loss, but introduces **bufferbloat (delay increases to 213 ms)**.

---

### Experiment 4: Propagation Delay Variation (5, 10, 20, 50 ms)

| Bottleneck Delay | Theoretical RTT | TCP Throughput | TCP Delay | UDP Throughput | UDP Delay |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **5 ms** | 18 ms | 0.890 Mbps | 134.56 ms | 0.962 Mbps | 133.87 ms |
| **10 ms** | 28 ms | 0.876 Mbps | 134.25 ms | 0.966 Mbps | 134.20 ms |
| **20 ms** | 48 ms | 0.884 Mbps | 154.04 ms | 0.962 Mbps | 148.92 ms |
| **50 ms** | 108 ms | 0.841 Mbps | 186.82 ms | 0.966 Mbps | 166.36 ms |

---

## 5. Wireshark PCAP Verification

Open the bottleneck PCAP file in Wireshark:
```bash
open -a Wireshark tcp-udp-bottleneck-0-0.pcap
```

### Analysis Filters:
- `tcp.port == 5000`: Displays TCP packets, ACKs, and TCP Fast Retransmissions / Timeouts.
- `udp.port == 6000`: Displays continuous UDP datagram stream.
- `tcp.analysis.retransmission`: Highlights dropped TCP packets retransmitted due to bottleneck buffer overflow.
