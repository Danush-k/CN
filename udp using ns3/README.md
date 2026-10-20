# UDP Traffic and Port-Based Application Delivery in NS-3

Simulates UDP communication between two nodes over a Point-to-Point link in NS-3, analyzing the effect of link latency on throughput, RTT, and Packet Delivery Ratio (PDR), while demonstrating port-based multiplexing with PCAP tracing for Wireshark.

---

## Topology

```text
Node 0 (10.1.1.1) ─────────────────────────── Node 1 (10.1.1.2)
  [UdpClient 1: Port 5000]       P2P Link       [UdpServer 1: Port 5000]
  [UdpClient 2: Port 6000]    20Mbps, Delay     [UdpServer 2: Port 6000]
```

---

## Files in this Folder

- [udp_latency_throughput.cc](file:///Users/danush/Desktop/SEM-5/CN-LAB/udp%20using%20ns3/udp_latency_throughput.cc): Complete NS-3 C++ simulation source code with Latency, RTT, PDR, and Throughput reporting.
- [run.sh](file:///Users/danush/Desktop/SEM-5/CN-LAB/udp%20using%20ns3/run.sh): Easy one-line runner script for all experiments.
- `udp-assignment-0-0.pcap`: Packet capture at Node 0 (Sender).
- `udp-assignment-1-0.pcap`: Packet capture at Node 1 (Receiver).

---

## How to Run

Navigate into this directory:
```bash
cd "/Users/danush/Desktop/SEM-5/CN-LAB/udp using ns3"
```

### 1. Run with Default (2 ms Latency, Dual Flows):
```bash
./run.sh 2
```

### 2. Run Single Flow (Step 3):
```bash
./run.sh single 2
./run.sh single 5
./run.sh single 10
./run.sh single 20
```

### 3. Run All Latencies at once:
```bash
./run.sh all
```

---

## Observation Tables

### Step 3: Single UDP Flow (Port 5000)

| Link Latency | Theoretical RTT ($2 \times \text{delay}$) | Packets Rx / Lost | PDR (%) | Throughput |
| :---: | :---: | :---: | :---: | :---: |
| **2 ms** | 4 ms | 5492 / 0 | **100.00%** | **4.9989 Mbps** |
| **5 ms** | 10 ms | 5490 / 0 | **100.00%** | **4.9971 Mbps** |
| **10 ms** | 20 ms | 5487 / 0 | **100.00%** | **4.9944 Mbps** |
| **20 ms** | 40 ms | 5481 / 0 | **100.00%** | **4.9889 Mbps** |

---

### Step 4 & 7: Two UDP Flows (Port 5000 & Port 6000)

| Link Latency | RTT | Flow 1 Throughput | Flow 2 Throughput | Total Throughput | Overall PDR |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **2 ms** | 4 ms | 4.9989 Mbps | 4.9989 Mbps | **9.9978 Mbps** | **100.00%** |
| **5 ms** | 10 ms | 4.9971 Mbps | 4.9971 Mbps | **9.9942 Mbps** | **100.00%** |
| **10 ms** | 20 ms | 4.9944 Mbps | 4.9944 Mbps | **9.9888 Mbps** | **100.00%** |
| **20 ms** | 40 ms | 4.9889 Mbps | 4.9889 Mbps | **9.9778 Mbps** | **100.00%** |

---

## Wireshark Packet Inspection

Open the receiver PCAP in Wireshark:
```bash
open -a Wireshark udp-assignment-1-0.pcap
```

### Wireshark Filters:
- `udp.port == 5000` $\rightarrow$ Shows packets delivered to Server 1.
- `udp.port == 6000` $\rightarrow$ Shows packets delivered to Server 2.
- `udp` $\rightarrow$ Shows both flows interleaving simultaneously over the same link.
