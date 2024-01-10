# Use an official Ubuntu image with tag "latest" as a parent image
FROM ubuntu:rolling

# Install necessary dependencies
RUN apt-get update && \
    apt-get install -y build-essential git libhwy-dev libjpeg-turbo8-dev libpng-dev cmake ninja-build ffmpeg liblcms2-2 liblcms2-dev liblcms2-utils

# Copy the source code into the container
COPY ./src /app/src

# Create a build directory
RUN mkdir build

# Change to the build directory
WORKDIR /app/build

# Run cmake and ninja to build the project
RUN cmake ../src -G Ninja && \
    ninja ssimulacra2

# Move the ssimulacra2 binary to /app
RUN mv ssimulacra2 /app/ssimulacra2

# Copy the video comparison script into the container
COPY compare_videos.sh /app/compare_videos.sh

# Make the script executable
RUN chmod +x /app/compare_videos.sh

# Change to the build directory
WORKDIR /app

# Set the entry point for the container
ENTRYPOINT ["/app/compare_videos.sh"]
