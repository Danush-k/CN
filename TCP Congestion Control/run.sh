#!/bin/bash
# Runner script for TCP Congestion Control Comparison in NS-3
# Compares TCP Reno, TCP Cubic, and TCP BBR in a VANET ITS Scenario

NS3_DIR="/Users/danush/ns-allinone-3.42/ns-3.42"
CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Copy latest cc file to NS-3 scratch folder
cp "$CURRENT_DIR/tcp_congestion.cc" "$NS3_DIR/scratch/"

run_variant() {
    VARIANT="$1"
    echo "=========================================================="
    echo " RUNNING TCP CONGESTION SIMULATION: $VARIANT"
    echo "=========================================================="
    (cd "$NS3_DIR" && ./ns3 run "scratch/tcp_congestion.cc --tcpVariant=$VARIANT")
    
    # Copy CWND CSV and XML files to current directory
    cp "$NS3_DIR"/tcp-"$VARIANT"-cwnd.csv "$CURRENT_DIR/" 2>/dev/null || true
    cp "$NS3_DIR"/tcp-"$VARIANT"-results.xml "$CURRENT_DIR/" 2>/dev/null || true
}

case "$1" in
    reno|TcpReno)
        run_variant "TcpReno"
        ;;
    cubic|TcpCubic)
        run_variant "TcpCubic"
        ;;
    bbr|TcpBbr)
        run_variant "TcpBbr"
        ;;
    all)
        run_variant "TcpReno"
        echo ""
        run_variant "TcpCubic"
        echo ""
        run_variant "TcpBbr"
        ;;
    *)
        echo "Usage: ./run.sh [reno | cubic | bbr | all]"
        echo "Running all 3 variants by default..."
        run_variant "TcpReno"
        echo ""
        run_variant "TcpCubic"
        echo ""
        run_variant "TcpBbr"
        ;;
esac

# Copy PCAP traces
cp "$NS3_DIR"/tcp-congestion-*.pcap "$CURRENT_DIR/" 2>/dev/null || true
echo ""
echo "Simulation completed! CWND logs, XML reports, and PCAPs copied to: $CURRENT_DIR"
