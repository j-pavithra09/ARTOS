#!/usr/bin/env python3
"""
extract_kitti.py
---------------------------------------
Extract obstacle distances from the KITTI
LiDAR dataset and generate kitti_trace.h
for the CERTOX RTOS.

Author : CERTOX Team
"""

import os
import glob
import numpy as np

# -------------------------------------------------
# Configuration
# -------------------------------------------------

KITTI_PATH = "../dataset/2011_09_26_drive_0001_sync/velodyne_points/data"
OUTPUT_FILE = "../kitti_trace.h"

# Maximum frames to process
MAX_FRAMES = 108

# Forward detection region
MIN_FORWARD = 0.0       # metres
MAX_FORWARD = 50.0      # metres
LANE_WIDTH = 1.5        # metres

# Ignore very small distances (noise)
MIN_DISTANCE = 0.5      # metres

# Safety Zones
CRITICAL = 6.0
WARNING = 15.0


# -------------------------------------------------
# Read one LiDAR frame
# -------------------------------------------------

def process_frame(file_path):

    points = np.fromfile(
        file_path,
        dtype=np.float32
    ).reshape(-1, 4)

    x = points[:, 0]
    y = points[:, 1]

    mask = (
        (x > MIN_FORWARD) &
        (x < MAX_FORWARD) &
        (np.abs(y) < LANE_WIDTH)
    )

    forward_points = points[mask]

    if len(forward_points) == 0:
        return MAX_FORWARD

    distance = np.min(forward_points[:, 0])

    if distance < MIN_DISTANCE:
        distance = MIN_DISTANCE

    return round(float(distance), 2)


# -------------------------------------------------
# Extract all frames
# -------------------------------------------------

def extract_dataset():

    files = sorted(
        glob.glob(
            os.path.join(KITTI_PATH, "*.bin")
        )
    )

    if len(files) == 0:
        raise FileNotFoundError(
            "No KITTI .bin files found."
        )

    files = files[:MAX_FRAMES]

    distances = []

    print("-----------------------------------")
    print("Reading KITTI LiDAR Dataset")
    print("-----------------------------------")

    for i, file in enumerate(files):

        d = process_frame(file)
        distances.append(d)

        if i % 10 == 0:
            print(
                f"Frame {i:03d} -> {d:.2f} m"
            )

    return distances


# -------------------------------------------------
# Generate Header
# -------------------------------------------------

def generate_header(distances):

    critical = 0
    warning = 0
    safe = 0

    with open(OUTPUT_FILE, "w") as f:

        f.write("/*\n")
        f.write(" * AUTO GENERATED FILE\n")
        f.write(" * DO NOT EDIT\n")
        f.write(" */\n\n")

        f.write("#ifndef KITTI_TRACE_H\n")
        f.write("#define KITTI_TRACE_H\n\n")

        f.write("#include <stdint.h>\n\n")

        f.write(
            f"#define KITTI_FRAMES {len(distances)}\n\n"
        )

        f.write(
            "static const float kitti_distance[] = {\n"
        )

        for i, d in enumerate(distances):

            if d < CRITICAL:
                critical += 1
            elif d < WARNING:
                warning += 1
            else:
                safe += 1

            f.write(f"    {d:.2f}f")

            if i != len(distances) - 1:
                f.write(",")

            f.write("\n")

        f.write("};\n\n")

        f.write("static int kitti_index = 0;\n\n")

        f.write(
"""static inline float kitti_get_distance(void)
{
    return kitti_distance[kitti_index];
}

static inline void kitti_next_frame(void)
{
    kitti_index++;

    if(kitti_index >= KITTI_FRAMES)
        kitti_index = 0;
}

static inline void kitti_reset(void)
{
    kitti_index = 0;
}
"""
)

        f.write("\n#endif\n")

    print("\nHeader generated successfully!\n")

    print("----------------------------")
    print("Statistics")
    print("----------------------------")

    print("Frames      :", len(distances))
    print("Critical    :", critical)
    print("Warning     :", warning)
    print("Safe        :", safe)

    print("Minimum     :", min(distances), "m")
    print("Maximum     :", max(distances), "m")
    print(
        "Average     :",
        round(sum(distances) / len(distances), 2),
        "m"
    )


# -------------------------------------------------
# Main
# -------------------------------------------------

def main():

    print("\nCERTOX KITTI Extractor\n")

    distances = extract_dataset()

    generate_header(distances)

    print("\nDone.")
    print("Generated:", OUTPUT_FILE)


if __name__ == "__main__":
    main()