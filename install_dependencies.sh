#!/bin/bash

# This script is only intended for those who haven't
# installed some necessary dependencies for running MLX42

# Exit on error
set -e

echo "Updating package list..."
sudo apt update

echo "Installing essential build tools and libraries..."
sudo apt install -y \
  cmake \
  build-essential \
  cmake \
  pkg-config \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libx11-dev \
  libxrandr-dev \
  libxinerama-dev \
  libxcursor-dev \
  libxi-dev \
  libxext-dev \
  libglfw3-dev \
  freeglut3-dev

echo "All dependencies installed successfully!"

