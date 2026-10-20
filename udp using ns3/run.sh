#!/bin/bash
# Simple runner script for NS-3 UDP Latency & Throughput Experiment

NS3_DIR="/Users/danush/ns-allinone-3.42/ns-3.42"
CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# 1. Copy latest cc file to NS-3 scratch folder
cp "$CURRENT_DIR/udp_latency_throughput.cc" "$NS3_DIR/scratch/"

case "$1" in
    all)
        echo "=========================================================="
        echo " EXPERIMENT 1: SINGLE UDP FLOW (Port 5000)"
        echo "=========================================================="
        for lat in 2 5 10 20; do
            (cd "$NS3_DIR" && ./ns3 run "scratch/udp_latency_throughput.cc --latency=$lat --enableFlow2=false")
        done

        echo "=========================================================="
        echo " EXPERIMENT 2: TWO UDP FLOWS (Port 5000 & 6000)"
        echo "=========================================================="
        for lat in 2 5 10 20; do
            (cd "$NS3_DIR" && ./ns3 run "scratch/udp_latency_throughput.cc --latency=$lat --enableFlow2=true")
        done

        # Copy PCAP files to current folder
        cp "$NS3_DIR"/udp-assignment-*.pcap "$CURRENT_DIR/" 2>/dev/null || true
        echo ""
        echo "PCAP trace files copied to: $CURRENT_DIR"
        ;;
    single)
        LATENCY="${2:-2}"
        echo "Running 1 flow with latency ${LATENCY} ms..."
        (cd "$NS3_DIR" && ./ns3 run "scratch/udp_latency_throughput.cc --latency=$LATENCY --enableFlow2=false")
        cp "$NS3_DIR"/udp-assignment-*.pcap "$CURRENT_DIR/" 2>/dev/null || true
        ;;
    *)
        LATENCY="${1:-2}"
        echo "Running 2 flows with latency ${LATENCY} ms..."
        (cd "$NS3_DIR" && ./ns3 run "scratch/udp_latency_throughput.cc --latency=$LATENCY --enableFlow2=true")
        cp "$NS3_DIR"/udp-assignment-*.pcap "$CURRENT_DIR/" 2>/dev/null || true
        ;;
esac
