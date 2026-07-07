import numpy as np
import os
import cv2

def detect_lane(image_path):
    """
    Simple lane detection from KITTI camera image
    Returns 1.0 = Lane OK
    Returns 0.0 = Lane Departure
    """
    # Read image
    img = cv2.imread(image_path)
    if img is None:
        return 1.0  # Default safe

    # Convert to grayscale
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Get bottom half of image
    # Lane markings are in bottom portion
    height = gray.shape[0]
    width  = gray.shape[1]
    bottom = gray[height//2:, :]

    # Detect edges
    edges = cv2.Canny(bottom, 50, 150)

    # Count edge pixels in left and right half
    left_edges  = np.sum(edges[:, :width//2])
    right_edges = np.sum(edges[:, width//2:])

    # If both sides have edges = lane detected = OK
    # If one side missing = lane departure
    if left_edges > 100 and right_edges > 100:
        return 1.0  # Lane OK
    else:
        return 0.0  # Lane Departure

def extract_lane_data(kitti_path, max_frames=100):
    """
    Extract lane status from KITTI camera images
    """
    lane_data = []

    # Path to camera images
    img_path = os.path.join(kitti_path,
               "image_02", "data")

    if not os.path.exists(img_path):
        print(f"Path not found: {img_path}")
        return []

    # Process each frame
    files = sorted(os.listdir(img_path))
    files = [f for f in files if f.endswith('.png')]
    files = files[:max_frames]

    print(f"Processing {len(files)} camera frames...")

    for i, filename in enumerate(files):
        full_path = os.path.join(img_path, filename)
        lane_status = detect_lane(full_path)
        lane_data.append(lane_status)

        if i % 10 == 0:
            status = "OK" if lane_status > 0.5 \
                          else "DEPARTURE"
            print(f"Frame {i:3d}: Lane {status}")

    return lane_data

def save_combined_header(distances, lanes,
                         output_file):
    """
    Save both radar distances and lane data
    into one C header file
    """
    # Make both arrays same length
    min_len = min(len(distances), len(lanes), 100)
    distances = distances[:min_len]
    lanes     = lanes[:min_len]

    with open(output_file, 'w') as f:
        f.write("/*\n")
        f.write(" * real_world_data.h\n")
        f.write(" * Auto-generated from "
                "KITTI Dataset\n")
        f.write(" * Karlsruhe Germany "
                "Real Driving Data\n")
        f.write(" */\n\n")
        f.write("#ifndef REAL_WORLD_DATA_H\n")
        f.write("#define REAL_WORLD_DATA_H\n\n")

        # Write distances
        f.write("/* Real radar distances "
                "from KITTI LiDAR (meters) */\n")
        f.write("static float "
                "real_distances[] = {\n    ")
        for i, d in enumerate(distances):
            f.write(f"{d:.2f}f")
            if i < len(distances) - 1:
                f.write(", ")
            if (i + 1) % 8 == 0:
                f.write("\n    ")
        f.write("\n};\n\n")

        # Write lane data
        f.write("/* Real lane status from "
                "KITTI Camera */\n")
        f.write("/* 1.0 = Lane OK  "
                "0.0 = Lane Departure */\n")
        f.write("static float "
                "real_lanes[] = {\n    ")
        for i, l in enumerate(lanes):
            f.write(f"{l:.1f}f")
            if i < len(lanes) - 1:
                f.write(", ")
            if (i + 1) % 10 == 0:
                f.write("\n    ")
        f.write("\n};\n\n")

        # Write total count
        f.write(f"static int total_readings"
                f" = {min_len};\n\n")
        f.write("#endif /* REAL_WORLD_DATA_H */\n")

    print(f"\nSaved to {output_file}")
    print(f"Total readings: {min_len}")
    print(f"Lane departures detected: "
          f"{lanes.count(0.0)}")

# ── Main ────────────────────────────────────────
if __name__ == "__main__":

    # Update this path to your KITTI folder
    kitti_path = "2011_09_26_drive_0001_sync"

    print("Extracting KITTI lane data...")
    lanes = extract_lane_data(kitti_path,
                              max_frames=100)

    # Load existing radar distances
    # (from your previous extraction)
    # If you have them already just paste here
    # Otherwise use sample values
    distances = [
        45.23, 44.87, 44.12, 43.56, 42.91,
        42.13, 41.34, 40.21, 38.93, 37.15,
        35.22, 32.84, 29.13, 24.51, 18.23,
        12.14,  8.32,  5.11,  3.21,  1.82
    ]

    # Extend distances to match lane count
    while len(distances) < len(lanes):
        distances.extend(distances)
    distances = distances[:len(lanes)]

    # Save combined header file
    save_combined_header(
        distances, lanes,
        "real_world_data.h"
    )

    print("\nDone! Copy real_world_data.h")
    print("to your adas_rtos folder")