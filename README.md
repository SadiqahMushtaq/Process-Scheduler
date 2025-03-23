# Process Scheduler in C 🖥️  

## Overview  
This project implements a **simple process scheduler** in C, simulating various **CPU scheduling algorithms**. It defines data structures and functions to schedule processes using:  
- **First-Come-First-Served (FIFO)**  
- **Shortest Job First (SJF)**  
- **Shortest Time-to-Completion First (STCF)**  
- **Round Robin (RR)**  

The scheduler reads process information, schedules execution based on the selected algorithm, and calculates **performance metrics** such as **throughput, turnaround time, and response time**.  

## Features  
✅ Implements **multiple CPU scheduling algorithms**  
✅ Reads process input dynamically from standard input  
✅ Supports **multiple process details** (PID, runtime, arrival time)  
✅ Provides **performance analysis**  

## Repository Structure  
- **`Scheduler.c`** – Main implementation of the process scheduler  
- **`Makefile`** – Automates compilation and execution  

## Compilation & Execution  
### Build the project  
Use the `Makefile` to compile the scheduler:  
```bash
make
