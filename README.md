# Custom C++ Software Rasterizer

This is a student project still in development. Feel free to utilize this and my overly verbose comments for study or practice.

A 3D graphics engine built entirely from scratch in C++ using SDL2, with no external rendering or math libraries. Every part of the pipeline, from rotating a point in 3D space to deciding which pixel gets colored, is implemented and derived by hand. The focus throughout was understanding why each piece of math works, not just calling a library function that does it.

## Features

- **Vertex/edge/triangle geometry model.** Shapes are represented generically as a list of vertices, edges, and triangles, so the same rendering code works for a hand built cube or an imported model with thousands of faces.
- **3D rotation.** Rotation around all three axes, derived from first principles (basis vectors and the unit circle) rather than copied from a reference implementation.
- **Perspective projection.** A simulated camera that scales objects based on depth, so farther geometry appears smaller.
- **Mouse driven orbit camera.** Click and drag to rotate the view in real time.
- **Diffuse lighting.** Per triangle surface normals (via cross product) compared against a fixed light direction (via dot product) to compute brightness.
- **Z-buffering.** Per pixel depth testing so overlapping triangles are drawn in the correct front to back order.
- **Triangle rasterization via barycentric coordinates.** Filled triangles use an edge function/barycentric approach, which also drives the depth interpolation used by the z-buffer.
- **Bresenham line drawing.** Integer only line rasterization for wireframe rendering.
- **Custom OBJ file parser.** Loads external triangulated .obj models (for example, from Blender) into the engine's native geometry format, with automatic bounding box based scaling so models of any source scale render at a consistent size.
- **Manual pixel pipeline.** Direct manipulation of an ABGR8888 pixel buffer, streamed to the screen via an SDL texture.

## Known Limitations

- Camera rotation is implemented as three sequential Euler angle rotations (pitch, yaw, roll applied in a fixed order). This can produce axis interference at extreme pitch angles. This is a well known limitation of Euler angle composition, as opposed to a quaternion based rotation system.
- The OBJ parser currently assumes pre-triangulated input (Blender's "triangulate faces" export option) and does not yet handle arbitrary n-gon faces.

## Project Structure

- `shapes.h`: core geometry data types (Vertex, Edge, Triangle, Shape) and shared constants.
- `transform.h` / `transform.cpp`: rotation, projection, and lighting/normal math.
- `render.h` / `render.cpp`: rasterization. Line drawing, triangle filling, z-buffering, and the OBJ file loader.
- `main.cpp`: window and event setup (SDL2), the render loop, and camera input handling.

## Getting Started

### Prerequisites

- A C++ compiler supporting C++17 or later
- CMake (3.10+)
- SDL2 development libraries

### Building and Running

```
cmake -S . -B build
cmake --build build
./build/rasterizer
```

### Controls

- Click and drag: orbit the camera around the model

## Planned Improvements

- A simple in app UI for adjusting parameters (rotation, lighting) at runtime
- A more flexible OBJ parser that handles n-gon triangulation automatically
