# KITTI Dataset Extraction

## What is KITTI?

KITTI is a real world autonomous driving
dataset collected from actual sensor
equipped vehicles driven on city roads
in Karlsruhe Germany.

- Created by: Karlsruhe Institute of Technology
- Year: 2012
- Used by: Google Waymo Tesla and CERTOX RTOS
- Citation: Geiger et al. IEEE CVPR 2013
- URL: cvlibs.net/datasets/kitti

## What Data We Use

| Data | Source | Used In |
|------|--------|---------|
| Radar distances | LiDAR velodyne | radar.c |
| Lane status | Camera + GPS/IMU | camera.c |

## Dataset Details

- Sequence: 2011_09_26_drive_0001
- Type: City driving
- Frames: 108
- Critical zones (below 6m): 24 frames
- Caution zones (6-15m): 77 frames
- Safe zones (above 15m): 7 frames
- Lane departures: 7 events

## How to Run With Real Dataset

### Step 1 — Install dependency
```bash
pip install numpy
```

### Step 2 — Download KITTI
### Step 3 — Run extraction
```bash
cd extraction
python3 extract_kitti.py
```

### Step 4 — Output