#  ARTOS - KITTI Dataset Extractor

## Overview

This utility extracts real obstacle distance information from the KITTI Autonomous Driving Dataset and converts it into a C header file (`kitti_trace.h`) that can be directly used by the CERTOX RTOS.

Instead of manually creating sensor values, the RTOS executes using realistic driving data collected from an actual vehicle.

---

# Dataset

Dataset:
KITTI Vision Benchmark Suite

Sequence:
2011_09_26_drive_0001_sync

Website:
https://www.cvlibs.net/datasets/kitti/

Sensors Used

• Velodyne HDL-64E LiDAR
• GPS / IMU (Optional)
• Stereo Camera (Not used in this extractor)

---

# Folder Structure

CERTOX/

├── dataset/
│   └── 2011_09_26_drive_0001_sync/
│       └── velodyne_points/
│           └── data/
│
├── tools/
│   ├── extract_kitti.py
│   └── README.md
│
├── kitti_trace.h
│
└── src/

---

# Requirements

Python 3.10+

NumPy

Install NumPy

pip install numpy

---

# How it Works

The extraction process performs the following steps:

1. Read every LiDAR (.bin) frame.

2. Remove all points behind the vehicle.

3. Keep only points inside the driving lane.

4. Ignore noisy measurements.

5. Find the nearest obstacle.

6. Store the distance.

7. Repeat for every frame.

8. Generate kitti_trace.h.

---

# Detection Region

Only objects inside the forward driving lane are considered.

Conditions:

Forward Distance

0 m < x < 50 m

Lane Width

-1.5 m < y < +1.5 m

This simulates a forward-facing automotive radar.

---

# Output

Running

python3 extract_kitti.py

produces

kitti_trace.h

which contains

• Number of frames

• Distance array

• Helper functions

• Frame index

---

# Using in CERTOX

Include

#include "kitti_trace.h"

Example

float distance = kitti_get_distance();

kitti_next_frame();

The RTOS reads one frame every scheduler tick.

---

# Safety Zones

Critical

Distance < 6 m

Immediate braking.

Warning

6 m ≤ Distance < 15 m

Slow down and monitor.

Safe

Distance ≥ 15 m

Continue normal driving.

---

# Example Output

CERTOX KITTI Extractor

Reading KITTI Dataset...

Frame 000 -> 4.21 m

Frame 001 -> 4.16 m

Frame 002 -> 4.09 m

...

Statistics

Frames : 108

Critical : 14

Warning : 61

Safe : 33

Generated:
kitti_trace.h

---

# Citation

A. Geiger,
P. Lenz,
R. Urtasun,

"Are We Ready for Autonomous Driving?
The KITTI Vision Benchmark Suite,"

IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 2012.

---
