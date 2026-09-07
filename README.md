# Custom C++ Software Rasterizer

This is a student project still in development. Feel free to utilize this and my overly verbose comments for study or practice.

A lightweight 3D graphics and line-drawing engine built entirely from scratch in **C++** using **SDL2**, focusing on deep mathematical understanding and low-level memory management without external graphics libraries.

## Features
- **Custom Coordinate System:** Translates standard Cartesian coordinates `(0,0)` to screen-space pixel buffers.
- **Manual Pixel Pipeline:** Direct memory manipulation of an `ABGR8888` pixel buffer streamed via SDL textures.

## Getting Started
### Prerequisites
- A C++ compiler (supporting C++17 or later)
- SDL2 development libraries

### Building and Running
```bash
# Example compilation command (adjust based on your setup)
g++ main.cpp -lSDL2 -o rasterizer
./rasterizer