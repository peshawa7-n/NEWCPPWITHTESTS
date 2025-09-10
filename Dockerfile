# Use a lightweight Python base image
FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Install required system packages
# - build-essential (C++ compiler)
# - cmake (for OpenCV build if needed)
# - ffmpeg (video processing)
# - libopencv-dev (OpenCV C++ dev files)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ffmpeg \
    libopencv-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Copy requirements.txt and install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy the whole project
COPY . .

# Compile the C++ video editor code
RUN g++ video_editor.cpp -o video_editor `pkg-config --cflags --libs opencv4`

# Command to run Python script by default
CMD ["python", "main.py"]
