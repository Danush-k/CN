# Simulation and Performance Analysis of TCP Congestion Control Algorithms in NS-3
### Intelligent Transportation System (VANET ITS Scenario)

Simulates a Vehicular Ad Hoc Network (VANET) where **5 Vehicles** stream real-time traffic updates, camera video, and road sensor data to a centralized **Traffic Control Server** through a **Roadside Router (RSU)** over a constrained bottleneck link.

---

## 1. Network Topology

```text
Vehicle 0 (10.0.1.1) ── (10 Mbps, 5 ms) ──┐
Vehicle 1 (10.0.2.1) ── (10 Mbps, 5 ms) ──┤
Vehicle 2 (10.0.3.1) ── (10 Mbps, 5 ms) ──┼─── [Roadside Router] ─── Bottleneck Link ─── [Traffic Control Server]
Vehicle 3 (10.0.4.1) ── (10 Mbps, 5 ms) ──┤      (10.0.6.1)     (5 Mbps, 20 ms, 50p)         (10.0.6.2:5000)
Vehicle 4 (10.0.5.1) ── (10 Mbps, 5 ms) ──┘
```

- **Vehicle Access Links**: $10\text{ Mbps}$, $5\text{ ms}$ delay
- **Bottleneck Link (Router ↔ Server)**: $5\text{ Mbps}$, $20\text{ ms}$ delay, DropTail queue ($50\text{ packets}$)
- **Traffic Type**: 5 concurrent TCP bulk streams transmitting $1024\text{-byte}$ packets
- **Simulation Duration**: $20.0\text{ seconds}$

---

## 2. Comparative Performance Analysis (Reno vs. Cubic vs. BBR)

Below are the empirical results measured using NS-3 FlowMonitor and CWND socket tracing on this topology:

| Performance Metric | TCP Reno (`TcpLinuxReno`) | TCP Cubic (`TcpCubic`) | TCP BBR (`TcpBbr`) |
| :--- | :---: | :---: | :---: |
| **Total Throughput** | **5.037 Mbps** | **5.045 Mbps** | **4.974 Mbps** |
| **Average End-to-End Delay** | **85.59 ms** | 93.22 ms | 104.59 ms |
| **Packet Loss Rate** | **0.95 %** (192 lost) | 2.16 % (442 lost) | 8.09 % (1743 lost) |
| **Jain's Fairness Index** | 0.955 | **1.000 (Perfect)** | 0.995 |
| **Average CWND** | 51,232 bytes (~50 pkts) | 24,015 bytes (~23 pkts) | Rate-paced (~1 MB) |
| **Maximum CWND** | 154,904 bytes | 72,360 bytes | 4,294,967,295 bytes |
| **Congestion Control Signal** | Packet Loss (AIMD) | Packet Loss (Cubic curve) | BtlBw & RTprop (Model) |

---

## 3. Detailed Comparison of TCP Variants

### 1. TCP Reno (Loss-Based AIMD)
- **Mechanism**: Increases congestion window by 1 packet per RTT during Congestion Avoidance, and halves the window upon packet loss (Additive Increase, Multiplicative Decrease).
- **Behavior**: Conservative and stable in small networks. Yields the lowest packet loss ($0.95\%$) and lowest average delay ($85.59\text{ ms}$) because it backs off aggressively whenever router queues fill up.
- **Limitation**: Suffers from slow window recovery and unfairness toward flows with varying RTTs.

### 2. TCP Cubic (Default in Linux & Android)
- **Mechanism**: Replaces linear growth with a cubic function of time $W(t) = C(t - K)^3 + W_{max}$, independent of RTT.
- **Behavior**: Achieved **perfect fairness (Jain's Index = 1.000)** across all 5 vehicles (~$1.0\text{ Mbps}$ per vehicle). It ramps up quickly, flattens near the prior saturation point ($W_{max}$), and explores for extra bandwidth smoothly.
- **Limitation**: Can cause moderate bufferbloat in drop-tail queues before drops occur.

### 3. TCP BBR (Bottleneck Bandwidth and Round-trip propagation time)
- **Mechanism**: Model-based congestion control developed by Google. Instead of treating packet loss as congestion, it continuously measures maximum delivery rate and minimum round-trip time.
- **Behavior**: Paces transmissions to match physical bottleneck bandwidth ($4.974\text{ Mbps}$). Maintains high link utilization even when packet loss occurs.
- **Limitation**: In standard DropTail FIFO queues with shallow buffers, BBR's aggressive probing phases cause higher loss ($8.09\%$) compared to Reno/Cubic.

---

## 4. Why Low Delay & Reliable Communication Are Crucial for Intelligent Transportation Systems (ITS)

1. **Safety-Critical Latency Deadlines (< 100 ms)**:
   - Applications like **Forward Collision Warning (FCW)**, **Emergency Electronic Brake Lights (EEBL)**, and **Intersection Collision Avoidance** require sub-100 ms delivery.
   - If congestion control induces high queuing delay or bufferbloat, warning messages arrive too late, compromising vehicular safety.

2. **Sensor Freshness & Real-Time Situational Awareness**:
   - Vehicles stream live camera feeds, LiDAR point clouds, and radar telemetry to traffic servers for dynamic signal optimization and hazard warnings.
   - Stale data caused by high latency is useless for automated decision-making.

3. **Fairness Among Vehicles**:
   - In a multi-vehicle corridor, congestion control must allocate bottleneck bandwidth equitably. If one vehicle starves another (poor fairness), traffic management loses situational awareness of that vehicle's section of the road.

---

## 5. Files in this Directory

- [tcp_congestion.cc](file:///Users/danush/Desktop/SEM-5/CN-LAB/TCP%20Congestion%20Control/tcp_congestion.cc): Complete NS-3 simulation code with FlowMonitor, CWND logging, and Jain's Fairness calculation.
- [run.sh](file:///Users/danush/Desktop/SEM-5/CN-LAB/TCP%20Congestion%20Control/run.sh): Runner script for Reno, Cubic, BBR, or all.
- `tcp-TcpReno-cwnd.csv`: Time vs. CWND log for TCP Reno.
- `tcp-TcpCubic-cwnd.csv`: Time vs. CWND log for TCP Cubic.
- `tcp-TcpBbr-cwnd.csv`: Time vs. CWND log for TCP BBR.
- `tcp-*-results.xml`: FlowMonitor XML reports.
- `tcp-congestion-*.pcap`: PCAP traces on access and bottleneck links.

---

## 6. How to Run

Navigate into this directory:
```bash
cd "/Users/danush/Desktop/SEM-5/CN-LAB/TCP Congestion Control"
```

### Run All 3 Variants (Reno, Cubic, BBR):
```bash
./run.sh all
```

### Run an Individual Variant:
```bash
./run.sh reno     # Runs TCP Reno
./run.sh cubic    # Runs TCP Cubic
./run.sh bbr      # Runs TCP BBR
```

---

## 7. Wireshark PCAP Verification

Open the bottleneck PCAP trace:
```bash
open -a Wireshark tcp-congestion-bottleneck-0-0.pcap
```

### Key Wireshark Filters:
- `tcp.analysis.retransmission`: Inspect packet drops and retransmission events at the bottleneck queue.
- `tcp.port == 5000`: View interleaved streams from all 5 vehicles.
- In Wireshark: **Statistics $\rightarrow$ TCP Stream Graphs $\rightarrow$ Time-Sequence (Stevens) / Round Trip Time Graph** to visually observe CWND growth and backoff behavior.
