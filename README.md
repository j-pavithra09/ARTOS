# ARTOS -Automotive Real-Time Operating System for ADAS Simulation

Simulative Evaluation of a Custom Lightweight RTOS Kernel Designed for ADAS Co-Processing

ARTOS is a lightweight educational Real-Time Operating System (RTOS) developed for the simulation and evaluation of Advanced Driver Assistance Systems (ADAS).

The project focuses on evaluating different real-time scheduling algorithms under identical ADAS workloads using real driving data extracted from the KITTI Autonomous Driving Dataset.

---

# Features

- Lightweight RTOS Kernel
- Static Task Management
- Priority-based Preemptive Scheduling
- Multiple Scheduling Algorithms
- KITTI Dataset Integration
- ADAS Task Simulation
- Deadline Monitoring
- Performance Evaluation
- Real-time Statistics
- QEMU Compatible

---

# Supported Scheduling Algorithms

ARTOS allows the scheduler to be changed without modifying the application layer.

Currently supported algorithms include:

| Scheduler | Description |
|-----------|-------------|
| Round Robin | Equal CPU time for all tasks |
| Fixed Priority Scheduling | Highest priority task always executes first |
| Rate Monotonic Scheduling (RMS) | Static priority based on task period |
| Earliest Deadline First (EDF) | Dynamic priority based on nearest deadline |
| Mixed Criticality Scheduling (MCS) | Prioritizes safety-critical automotive tasks |

The scheduler can be selected at compile time for performance comparison.

---

# ADAS Tasks

The RTOS simulates common ADAS workloads including:

- Radar Distance Monitoring
- Lane Detection
- Obstacle Detection
- Emergency Braking
- Dashboard Display

Each task has

- Period
- Worst Case Execution Time (WCET)
- Priority
- Deadline

---

# KITTI Dataset Integration

ARTOS uses the KITTI Autonomous Driving Dataset.

Instead of manually entering sensor values, a Python extraction tool automatically converts KITTI frames into C arrays.

The generated data includes

- Obstacle distance
- Lane status
- Frame sequence

Generated file:

```
kitti_trace.h
```

---

# Folder Structure

```
ARTOS/
│
├── kernel/
│   ├── scheduler.c
│   ├── task.c
│   ├── queue.c
│   └── ...
│
├── apps/
│   ├── radar_task.c
│   ├── brake_task.c
│   ├── lane_task.c
│   └── dashboard_task.c
│
├── scripts/
│   └── extract_kitti.py
│
├── dataset/
│
├── include/
│
├── kitti_trace.h
│
└── README.md
```

---

# Performance Metrics

ARTOS records the following metrics during execution.

- CPU Utilization
- Deadline Misses
- Response Time
- Waiting Time
- Turnaround Time
- Scheduler Overhead
- Context Switch Count
- Task Execution Time

These metrics are compared across different scheduling algorithms.

---

# Building

Clone the repository

```
git clone https://github.com/j-pavithra09/ARTOS.git
```

Enter the project

```
cd ARTOS
```

Compile

```
make
```

Run

```
./artos
```

---

# Using the KITTI Dataset

Download

```
KITTI Raw Dataset
Sequence:
2011_09_26_drive_0001_sync
```

Place the dataset inside the project.

```
ARTOS/
dataset/
2011_09_26_drive_0001_sync/
```

Run

```
python3 scripts/extract_kitti.py
```

This generates

```
kitti_trace.h
```

which is automatically used by the RTOS.

---

# Research Objective

The objective of ARTOS is to compare different scheduling algorithms for automotive workloads using identical sensor data.

The project evaluates how each scheduling algorithm performs under

- Normal workload
- High CPU utilization
- Safety-critical conditions
- Deadline-sensitive execution

---

# Future Improvements

- Multi-core Scheduling
- CAN Bus Simulation
- ISO 26262 Safety Extensions
- Memory Protection
- Real-Time Visualization
- Fault Injection Framework
- Energy-Aware Scheduling

---

# Technologies Used

- C
- Python
- GCC
- Make
- QEMU
- KITTI Dataset

---

# Authors

Developed as an academic research project for the simulation and evaluation of real-time scheduling algorithms in ADAS systems.

```
