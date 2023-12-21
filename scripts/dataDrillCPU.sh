#!/bin/bash

processName="dataDrill"

# Get PIDs of the process
pids=$(pidof $processName)

if [ -z "$pids" ]; then
    echo "Process not found."
    exit 1
fi

totalCpuTime=0

# Read /proc/[pid]/stat for each pid to get the process CPU time
for pid in $pids; do
    if [ -f /proc/$pid/stat ]; then
        # utime + stime
        cpuTime=$(awk '{print $14 + $15}' /proc/$pid/stat)
        totalCpuTime=$((totalCpuTime + cpuTime))
    fi
done

# Get system uptime in seconds
uptime=$(awk '{print $1}' /proc/uptime)

# Get number of CPU cores
cpuCores=$(grep -c processor /proc/cpuinfo)

# Calculate total CPU usage
# Formula: totalCpuTime / (uptime * cpuCores)
cpuUsage=$(awk -v totalCpuTime="$totalCpuTime" -v uptime="$uptime" -v cpuCores="$cpuCores" 'BEGIN {printf "%.2f", (totalCpuTime / uptime / cpuCores) * 100}')

echo "$cpuUsage"
