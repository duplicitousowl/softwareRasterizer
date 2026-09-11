#pragma once
#include <vector>


// setting up the window size
    const int W_WIDTH = 800;
    const int W_HEIGHT = 800;

// make helper struct for 2d screen coordinates
struct Point {
    int x;
    int y;
};

// Edge struct, will hold index of vertices in vector<vertex> that have an edge connecting them 
struct Edge {
    int a, b;
};

struct Vertex {
    float x, y, z;
};

struct Triangle {
    int a, b, c;
};

// shape struct to hold a shape's vertices and edge relationship data
struct Shape {
    std::vector<Edge> edges;
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};