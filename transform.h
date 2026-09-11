#pragma once
#include "shapes.h"

Vertex rotate_x (Vertex v, float theta);

Vertex rotate_y(Vertex v, float theta);

Vertex rotate_z(Vertex v, float theta);

Vertex rotate3d(Vertex v, float theta_x, float theta_y, float theta_z);

Shape rotate_shape(Shape shape, float theta_x, float theta_y, float theta_z);

Vertex project_vertex (Vertex v, float d);

Shape project_shape(Shape shape, float d);
