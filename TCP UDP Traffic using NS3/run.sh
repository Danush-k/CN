#!/bin/bash
# Runner script for TCP/UDP Traffic Bottleneck Simulation in NS-3

NS3_DIR="/Users/danush/ns-allinone-3.42/ns-3.42"
CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# 1. Copy latest cc file to NS-3 scratch folder
cp "$CURRENT_DIR/tcp_udp_traffic.cc" "$NS3_DIR/scratch/"

run_sim() {
    (cd "$NS3_DIR" && ./ns3 run "scratch/tcp_udp_traffic.cc $1")
}

case "$1" in
    exp1)
        echo "=========================================================="
        echo " EXPERIMENT 1: BOTTLENECK BANDWIDTH (2, 5, 10 Mbps)"
        echo "=========================================================="
        for bw in 2 5 10; do
            echo ""
            echo ">>> Running with Bottleneck Bandwidth: ${bw} Mbps..."
            run_sim "--bottleneckRate=$bw"
        done
        ;;
    exp2)
        echo "=========================================================="
        echo " EXPERIMENT 2: UDP TRAFFIC RATE (0.5, 1, 2, 5 Mbps)"
        echo "=========================================================="
        for rate in 0.5 1.0 2.0 5.0; do
            echo ""
            echo ">>> Running with UDP Traffic Rate: ${rate} Mbps..."
            run_sim "--udpRate=$rate"
        done
        ;;
    exp3)
        echo "=========================================================="
        echo " EXPERIMENT 3: QUEUE SIZE (20, 50, 100 packets)"
        echo "=========================================================="
        for q in 20 50 100; do
            echo ""
            echo ">>> Running with DropTail Queue Size: ${q} packets..."
            run_sim "--queueSize=$q"
        done
        ;;
    exp4)
        echo "=========================================================="
        echo " EXPERIMENT 4: PROPAGATION DELAY (5, 10, 20, 50 ms)"
        echo "=========================================================="
        for del in 5 10 20 50; do
            echo ""
            echo ">>> Running with Bottleneck Delay: ${del} ms..."
            run_sim "--bottleneckDelay=$del"
        done
        ;;
    all)
        echo "Running all experiments (1, 2, 3, 4)..."
        "$0" exp1
        "$0" exp2
        "$0" exp3
        "$0" exp4
        ;;
    *)
        echo "Running Default Configuration (2 Mbps Bottleneck, 10 ms Delay, 1 Mbps UDP, 50-pkt Queue)..."
        run_sim "$*"
        ;;
esac

# Copy generated PCAP files to current folder
cp "$NS3_DIR"/tcp-udp-*.pcap "$CURRENT_DIR/" 2>/dev/null || true
echo ""
echo "Simulation completed. PCAP trace files copied to: $CURRENT_DIR"
