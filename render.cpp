#include <iostream>
#include "render.h"
#include <cmath>
#include <cstdint>


//coordinate translator function to allow cartesian coordinate use ((0,0) center of screen)
Point c_to_screen(int x, int y) { 
    return {
        x + (W_WIDTH / 2),
        (W_HEIGHT / 2) - y
    };
}

// helper function to check bounds, then change the specified coordinate in the pixels' vectors' data (uint32_t color)
void put_pixel(int x, int y, uint32_t color, std::vector<uint32_t>& pixels) {
    if (x >= 0 && x < W_WIDTH && y >= 0 && y < W_HEIGHT) {
        pixels[(y * W_WIDTH) + x] = color;
    }
}

// Bresenham's line drawing algorithm
void draw_line(int x0, int y0, int x1, int y1, uint32_t color, std::vector<uint32_t>& pixels) {
    // dx and dy represent the total distance the line needs to travel on each axis respectively
    int dx = std::abs(x0 - x1);
    int dy = std::abs(y0 - y1);

    // determine the value of each step we will take determined by the point orientations
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    // instantiate the error. starting with dx - dy makes y the floor and x the ceiling
    int err = dx - dy;

    while (true) {
        // place a pixel at the current pixel at the start of every loop
        put_pixel(x0, y0, color, pixels);

        // check to see if the line has reached it's destination, if it has breaks
        if (x0 == x1 && y0 == y1) break;

        // doubles the error. This changes the trigger point to be an int, rather than .5. This allows for no float math.
        int e2 = err * 2;

        // if error threshold remains above the distance y needs to travel, subtracts the y debt to be back beneath it and increment x + or - 1. the error threshold being above the total distance y needs to travel means x's step takes precedence.
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        // if the error threshold remains below the distance our x needs to travel, adds the debt back to be above threshold and increments y + or - 1. the error threshold remaining below the distance x needs to travel means y's step takes precedence.
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}


void draw_shape(Shape shape, uint32_t color, std::vector<uint32_t>& pixels) {
    for (int i = 0; i < shape.edges.size(); ++i) {
        // unpacks the vertices that are specified in the edge relationship vector
        Vertex v0 = shape.vertices[shape.edges[i].a];
        Vertex v1 = shape.vertices[shape.edges[i].b];

        // points v0.x and v0.y can be floats from sin/cos rotations. round them, cast to int type, and convert from cartesian coordinates to screen for draw line function
        Point s0 = c_to_screen(static_cast<int>(std::round(v0.x)), static_cast<int>(std::round(v0.y)));
        Point s1 = c_to_screen(static_cast<int>(std::round(v1.x)), static_cast<int>(std::round(v1.y)));

        draw_line(s0.x, s0.y, s1.x, s1.y, color, pixels);
    }
}

// the vertices passed have all been flattened into 2d at this point by project_shape, and therefore fulfill the Point struct
int edge_function (Point v0, Point v1, Point pixel) {
    // establish initial vector from edge denoted by v0 -> v1
    Point v = {v1.x - v0.x, v1.y - v0.y};
    // rotate vector 90 degrees with same (x,y) -> (-y, x) formula
    Point v_perp = {-v.y, v.x};
    // establish pixel vector
    Point pixel_vector = {pixel.x - v0.x, pixel.y - v0.y};
    // find dot product of pixel vector and the vector perpendicular to edge. When done with all 3 edges of triangle, all dot product results will be of same sign if inside triangle
    return (pixel_vector.x * v_perp.x) + (pixel_vector.y * v_perp.y);  
}

bool depth_test (int x, int y, float depth, std::vector<float>& z_buffer) {
    if (x >= 0 && x < W_WIDTH && y >= 0 && y < W_HEIGHT){
    if (z_buffer[(y * W_WIDTH) + x] > depth) {
        z_buffer[(y * W_WIDTH) + x] = depth;
        return true;
    }
}
    return false;
}

void draw_triangle(Point p1, Point p2, Point p3, float z1, float z2, float z3, uint32_t color, std::vector<uint32_t>& pixels, std::vector<float>& z_buffer) {
    int min_x = std::min(p1.x, std::min(p2.x, p3.x));
    int max_x = std::max(p1.x, std::max(p2.x, p3.x));
    int min_y = std::min(p1.y, std::min(p2.y, p3.y));
    int max_y = std::max(p1.y, std::max(p2.y, p3.y));

    for (int y = min_y; y < max_y; ++y) {
        for (int x = min_x; x < max_x; ++x) {
            Point point_to_eval = {x, y};
            int r1 = edge_function(p1, p2, point_to_eval);
            int r2 = edge_function(p2, p3, point_to_eval);
            int r3 = edge_function(p3, p1, point_to_eval);

            
            if ((r1 > 0 && r2 > 0 && r3 > 0) || (r1 < 0 && r2 < 0 && r3 < 0)) {
            float total = r1 + r2 + r3;
            float w1 = r2 / total;
            float w2 = r3 / total;
            float w3 = r1 / total;

            float depth = (z1 * w1) + (z2 * w2) + (z3 * w3);
            if (depth_test(x, y, depth, z_buffer)) {
                put_pixel(x, y, color, pixels);
            }
            }
        }
    }
}

void fill_shape(Shape shape, uint32_t color, std::vector<uint32_t>& pixels, std::vector<float>& z_buffer) {
    for (Triangle t : shape.triangles) {
        Point p1 = c_to_screen(static_cast<int>(std::round(shape.vertices[t.a].x)), static_cast<int>(std::round(shape.vertices[t.a].y)));
        Point p2 = c_to_screen(static_cast<int>(std::round(shape.vertices[t.b].x)), static_cast<int>(std::round(shape.vertices[t.b].y)));
        Point p3 = c_to_screen(static_cast<int>(std::round(shape.vertices[t.c].x)), static_cast<int>(std::round(shape.vertices[t.c].y)));

        float z1 = shape.vertices[t.a].z;
        float z2 = shape.vertices[t.b].z;
        float z3 = shape.vertices[t.c].z;

        draw_triangle(p1, p2, p3, z1, z2, z3, color, pixels, z_buffer);
    }
}