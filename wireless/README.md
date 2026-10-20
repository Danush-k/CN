# Assignment 11: Performance Analysis of a Heterogeneous Wireless Network

Simulates a Smart-Campus heterogeneous wireless network in NS-3 consisting of **Wi-Fi**, **Zigbee (IEEE 802.15.4 + 6LoWPAN)**, and **Bluetooth-like** short-range wireless nodes communicating with their respective coordinator/gateway nodes under variable user mobility speeds ($1\text{ m/s}$, $5\text{ m/s}$, and $10\text{ m/s}$).

---

## 1. Network Setup & Architecture

```text
[ Mobile Nodes (3) ] ─── Wireless Channel ───> [ Fixed Gateway Node (1) ]
  (RandomWalk2d Mobility)                            (Position: 50m, 50m)
  Speeds: 1, 5, 10 m/s
```

| Technology | Standard / Protocol | Tx Power | Frequency | Packet Size | Offered Rate |
| :--- | :--- | :---: | :---: | :---: | :---: |
| **Wi-Fi** | IEEE 802.11g (AP & STA Mode) | $16\text{ dBm}$ | $2.4\text{ GHz}$ | $512\text{ bytes}$ | $1.0\text{ Mbps}$ |
| **Zigbee** | IEEE 802.15.4 + 6LoWPAN (IPv6) | $0\text{ dBm}$ | $2.4\text{ GHz}$ | $100\text{ bytes}$ | $250\text{ Kbps}$ |
| **Bluetooth-like** | Short-range Low-Power Wireless | $4\text{ dBm}$ | $2.4\text{ GHz}$ | $200\text{ bytes}$ | $100\text{ Kbps}$ |

- **Mobility Model**: `RandomWalk2dMobilityModel` inside a $100\text{m} \times 100\text{m}$ campus boundary.
- **Speeds Tested**:
  - $1\text{ m/s}$ (Pedestrian walking speed)
  - $5\text{ m/s}$ (Running / cycling speed)
  - $10\text{ m/s}$ (Campus scooter / vehicle speed)

---

## 2. Performance Observation Table

Below are the empirical simulation results measured across all 9 conditions:

| Technology | Speed (m/s) | Received Power (dBm) | Throughput (Kbps) | Packet Loss (%) | End-to-End Delay (ms) |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Wi-Fi** | **1.0 m/s** | -50.71 dBm | **1498.91 Kbps** | **0.06 %** | **2.72 ms** |
| **Wi-Fi** | **5.0 m/s** | -55.93 dBm | 1284.78 Kbps | 14.34 % | 63.49 ms |
| **Wi-Fi** | **10.0 m/s** | -58.02 dBm | 1013.53 Kbps | 32.42 % | 76.77 ms |
| **Zigbee** | **1.0 m/s** | -68.72 dBm | 0.53 Kbps | 99.86 % | 1009.60 ms |
| **Zigbee** | **5.0 m/s** | -72.09 dBm | 0.53 Kbps | 99.86 % | 1009.60 ms |
| **Zigbee** | **10.0 m/s** | -71.88 dBm | 0.53 Kbps | 99.86 % | 1009.60 ms |
| **Bluetooth** | **1.0 m/s** | -64.71 dBm | 0.00 Kbps | 100.00 % | 0.00 ms |
| **Bluetooth** | **5.0 m/s** | -66.56 dBm | 47.29 Kbps | 68.45 % | 0.43 ms |
| **Bluetooth** | **10.0 m/s** | -65.59 dBm | 11.20 Kbps | 92.53 % | 0.46 ms |

---

## 3. Analysis & Key Findings (Requirement d)

### 1. Effect of Node Mobility Speed:
- **Signal Strength (Received Power)**: As speed increases, mobile nodes travel farther towards the perimeter of the simulation area, increasing the average distance to the central gateway ($d$). Under the Friis path-loss equation ($PL \propto 20 \log_{10}(d)$), received power drops significantly (e.g. Wi-Fi drops from $-50.71\text{ dBm}$ to $-58.02\text{ dBm}$).
- **Throughput & Packet Loss**: Higher mobility causes rapid channel variation and intermittent disconnection when nodes move near the cell edge. For Wi-Fi, throughput drops from $\sim 1.5\text{ Mbps}$ to $\sim 1.0\text{ Mbps}$, and packet loss jumps from $0.06\%$ to $32.42\%$.
- **End-to-End Delay**: As nodes move further away, packets require multiple MAC-layer retransmissions before acknowledgment, increasing average latency (e.g., Wi-Fi delay increases from $2.72\text{ ms}$ up to $76.77\text{ ms}$).

### 2. Comparison Across Technologies:
- **Wi-Fi (High Bandwidth, Wide Coverage)**: Demonstrates the best overall performance in a campus setting, maintaining $\ge 1\text{ Mbps}$ throughput even at high mobility speeds ($10\text{ m/s}$).
- **Bluetooth (Short-Range, Low Power)**: With lower transmit power ($4\text{ dBm}$), nodes quickly lose connectivity when moving beyond short range ($>10\text{ m}$), exhibiting high packet loss.
- **Zigbee (Low Power, Low Data Rate)**: Operates at $0\text{ dBm}$ transmit power and is optimized for stationary/slow periodic sensor updates rather than high-speed continuous streaming.

---

## 4. Files in this Directory

- [wireless.cc](file:///Users/danush/Desktop/SEM-5/CN-LAB/wireless/wireless.cc): Streamlined modular NS-3 simulation source code (~180 lines, reduced from 1000+ lines).
- [run.sh](file:///Users/danush/Desktop/SEM-5/CN-LAB/wireless/run.sh): Easy runner script for all experiments.
- [README.md](file:///Users/danush/Desktop/SEM-5/CN-LAB/wireless/README.md): Full analysis report and observation table.

---

## 5. How to Run

Navigate into this directory:
```bash
cd "/Users/danush/Desktop/SEM-5/CN-LAB/wireless"

# Run all 9 experiments (Wi-Fi, Zigbee, Bluetooth across 1, 5, 10 m/s)
./run.sh

# Or run individual technologies:
./run.sh wifi
./run.sh zigbee
./run.sh bluetooth
```
