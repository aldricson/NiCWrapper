#!/bin/bash

# Replace 'programName' with the actual name of your program's executable
program_name="dataDrill"

# Get the PID of your program
program_pid=$(pgrep -x "$program_name")

if [ -z "$program_pid" ]; then
  echo "Program is not running."
  exit 1
fi

# List all open TCP ports for the program
echo "Open TCP ports for program $program_name (PID: $program_pid):"
netstat -tuln | grep "$program_pid"

# Alternatively, you can use 'ss' instead of 'netstat'
# ss -tuln | grep "$program_pid"
