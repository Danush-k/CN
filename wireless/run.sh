#!/bin/bash
# Runner script for Assignment 11: Heterogeneous Wireless Network Performance

NS3_DIR="/Users/danush/ns-allinone-3.42/ns-3.42"
CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Copy latest cc file to NS-3 scratch folder
cp "$CURRENT_DIR/wireless.cc" "$NS3_DIR/scratch/"

case "$1" in
    wifi)
        echo "Running Wi-Fi experiment across speeds 1, 5, 10 m/s..."
        (cd "$NS3_DIR" && ./ns3 run "scratch/wireless.cc --tech=wifi")
        ;;
    zigbee)
        echo "Running Zigbee experiment across speeds 1, 5, 10 m/s..."
        (cd "$NS3_DIR" && ./ns3 run "scratch/wireless.cc --tech=zigbee")
        ;;
    bluetooth)
        echo "Running Bluetooth experiment across speeds 1, 5, 10 m/s..."
        (cd "$NS3_DIR" && ./ns3 run "scratch/wireless.cc --tech=bluetooth")
        ;;
    all|*)
        echo "Running all experiments (Wi-Fi, Zigbee, Bluetooth at 1, 5, 10 m/s)..."
        (cd "$NS3_DIR" && ./ns3 run "scratch/wireless.cc --tech=all")
        ;;
esac
