# Base Python image
FROM python:3.11-slim

WORKDIR /app

# Install system dependencies (OpenCV, C++ compiler)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libopencv-dev \
    python3-opencv \
    && rm -rf /var/lib/apt/lists/*

# Copy Python dependencies
COPY requirements.txt .

# Install Python packages
RUN pip install --no-cache-dir -r requirements.txt

# Copy all project files
COPY . .

# Compile the C++ video editor
RUN g++ video_editor.cpp -o video_editor `pkg-config --cflags --libs opencv4`

# Default run command: main.py
CMD ["python", "main.py"]
