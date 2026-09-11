#include "shapes.h"
#include <cmath>

// Define a rotate function that takes in a vertex, applies the rotation formula, and returns a new vertex to preserve original
Vertex rotate_x (Vertex v, float theta) {
    float yp = (v.y * std::cos(theta)) - (v.z * std::sin(theta));
    float zp = (v.y * std::sin(theta)) + (v.z * std::cos(theta));

    Vertex rotated_vertex = {v.x, yp, zp};
    return rotated_vertex;
}

Vertex rotate_y(Vertex v, float theta) {
    float zp = (v.z * std::cos(theta)) - (v.x * std::sin(theta));
    float xp = (v.z * std::sin(theta)) + (v.x * std::cos(theta));

    Vertex rotated_vertex = {xp, v.y, zp};
    return rotated_vertex;
}

Vertex rotate_z(Vertex v, float theta) {
    float xp = (v.x * std::cos(theta)) - (v.y * std::sin(theta));
    float yp = (v.x * std::sin(theta)) + (v.y * std::cos(theta));

    Vertex rotated_vertex = {xp, yp, v.z};
    return rotated_vertex;
}

Vertex rotate3d(Vertex v, float theta_x, float theta_y, float theta_z){
    return rotate_x(rotate_y(rotate_z(v, theta_z), theta_y), theta_x);
}

// Define a rotate shape function that loops over a shapes vertices, runs them through the rotate vertex function and returns a new rotated shape.
Shape rotate_shape(Shape shape, float theta_x, float theta_y, float theta_z) {
    Shape shape_to_return;
    shape_to_return.edges = shape.edges;
    shape_to_return.triangles = shape.triangles;

    for (int i = 0; i < shape.vertices.size(); ++i) {
        Vertex rotated_vertex;
        rotated_vertex = rotate3d(shape.vertices[i], theta_x, theta_y, theta_z);
        shape_to_return.vertices.push_back(rotated_vertex);
    }

    return shape_to_return;
}

Vertex project_vertex (Vertex v, float d) {
    float scale = d / (v.z + d);
    return {v.x * scale, v.y * scale, v.z};

}

Shape project_shape(Shape shape, float d) {
    for (int i = 0; i < shape.vertices.size(); ++i){
        shape.vertices[i] = project_vertex(shape.vertices[i], d);
    }
    return shape;
}