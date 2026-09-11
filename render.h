#pragma once
#include "shapes.h"
#include <cstdint>


Point c_to_screen(int x, int y);

void put_pixel(int x, int y, uint32_t color, std::vector<uint32_t>& pixels);

void draw_line(int x0, int y0, int x1, int y1, uint32_t color, std::vector<uint32_t>& pixels);

void draw_shape(Shape shape, uint32_t color, std::vector<uint32_t>& pixels);

int edge_function (Point v0, Point v1, Point pixel);

bool depth_test (int x, int y, float depth, std::vector<float>& z_buffer);

void draw_triangle(Point p1, Point p2, Point p3, float z1, float z2, float z3, uint32_t color, std::vector<uint32_t>& pixels, std::vector<float>& z_buffer);

void fill_shape(Shape shape, uint32_t color, std::vector<uint32_t>& pixels, std::vector<float>& z_buffer);
