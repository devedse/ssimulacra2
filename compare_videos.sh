#!/bin/bash

# Check if the correct number of arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 video1.mp4 video2.mp4"
    exit 1
fi

# Set input video file paths
video1=$1
video2=$2

# Create temporary directories for frames
mkdir /tmp/frames

# Extract frames from the first video
ffmpeg -i "$video1" -t 10 -q:v 2 -vf "select=not(mod(n\,10))" -vsync vfr -pix_fmt yuv420p10le "/tmp/frames/frame_%04d_a.png"

# Extract frames from the second video
ffmpeg -i "$video2" -t 10 -q:v 2 -vf "select=not(mod(n\,10))" -vsync vfr -pix_fmt yuv420p10le "/tmp/frames/frame_%04d_b.png"

# Get the list of frame files for comparison
frames1=$(ls /tmp/frames/frame_*_a.png)
frames2=$(ls /tmp/frames/frame_*_b.png)

# Calculate SSIM score for each pair of frames
for f1 in $frames1; do
    f2=$(echo "$f1" | sed 's/_a/_b/')  # Replace _a with _b in the filename
    score=$(./ssimulacra2 "$f1" "$f2")
    echo "SSIM score for $(basename "$f1") and $(basename "$f2"): $score"
done

# Clean up temporary directories
#rm -r /tmp/frames
