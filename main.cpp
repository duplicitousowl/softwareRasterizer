#include <iostream>
#include <vector>
#include <cstdint>
#include <SDL.h>
#include <cmath>
// to run this, in root directory: cmake --build build
//                                 ./build/rasterizer

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

// shape struct to hold a shape's vertices and edge relationship data
struct Shape {
    std::vector<Edge> edges;
    std::vector<Vertex> vertices;
};

// Define a rotate function that takes in a vertex, applies the rotation formula, and returns a new vertex to preserve original
Vertex rotate_z(Vertex v, float theta) {
    float xp = (v.x * std::cos(theta)) - (v.y * std::sin(theta));
    float yp = (v.x * std::sin(theta)) + (v.y * std::cos(theta));

    Vertex rotated_vertex = {xp, yp, v.z};
    return rotated_vertex;
}

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

Vertex rotate3d(Vertex v, float theta_x, float theta_y, float theta_z){
    return rotate_x(rotate_y(rotate_z(v, theta_z), theta_y), theta_x);
}

// Define a rotate shape function that loops over a shapes vertices, runs them through the rotate vertex function and returns a new rotated shape.
Shape rotate_shape(Shape shape, float theta_x, float theta_y, float theta_z) {
    Shape shape_to_return;
    shape_to_return.edges = shape.edges;

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

// for testing draw_shape
Shape make_test_box() {
    Shape s;
    s.vertices = {
        {-100, 100, -100},
        {-100, -100, -100},
        {100, -100, -100},
        {100, 100, -100},
        {-100, 100, 100},
        {-100, -100, 100},
        {100, -100, 100},
        {100, 100, 100}
    };
    s.edges = {
        {0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6}, {6,7},{7,4},{0,4},{1,5},{2,6},{3,7}
    };
    return s;
}



float theta = 0.00f;

int main() {    
    // Initialize pixel vector (480,000 pixels)
    std::vector<uint32_t> pixels(W_WIDTH * W_HEIGHT, 0xFFFFFFFF); 

    // initialize SDLwindow, SDLRenderer and a pointer to each set to null
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    
    // Create both the window and the renderer
    SDL_CreateWindowAndRenderer(
        W_WIDTH, W_HEIGHT, 0,
        &window, &renderer);
    
    // create texture pointer
    SDL_Texture* texture = SDL_CreateTexture(renderer,
    // pixels vector is storing ABGR32b values
                                            SDL_PIXELFORMAT_ABGR8888,
    // this is for updating pixel data with custom data i.e. the 'pixels' vector. enum of 1. alternatives are SDL_TEXTUREACCESS_STATIC (enum of 0) for static images, and SDL_TEXTUREACCESS_TARGET (enum of 2) for using SDL tools for pixel data
                                            SDL_TEXTUREACCESS_STREAMING,
                                            W_WIDTH,
                                            W_HEIGHT);
    
    // initialize true bool for while loop
    bool isRunning = true;
    // initialize SDLEvent to event
    SDL_Event event; 
    Shape test_box = make_test_box();

    // loop to keep window open/running
    while (isRunning) {
        // while loop to keep running until user x's out, setting outer loop to false
        // SDL_PollEvent requires address of event to poll for data, hence the &event
        // important to have all functions running after this second while loop to prevent them continuing to run and wasting resources once application is x'd out of.
        while(SDL_PollEvent(&event)) {
            // check for user x'ing out
            if (event.type == SDL_QUIT) {
                // if user x's out, ends loop
                isRunning = false; 
            }
        }   
            // reset vector at beginning of every loop so there's no lingering line data
            std::fill(pixels.begin(), pixels.end(), 0xFFFFFFFF);

            uint32_t black = 0xFF000000;
            theta -= 0.001f;

           draw_shape(project_shape(rotate_shape(test_box, theta, theta*.6f, theta*1.2f), 500), black, pixels);

            // clear the renderer in anticipation of filling it with the updated texture
            SDL_RenderClear(renderer);

            // updates SDL texture with the 'pixels' vector data. 
            SDL_UpdateTexture(texture,
                            // null since this isn't using rect; Since it's updating entire texture/renderer, dont need to specify a region to update
                            nullptr,
                            // requires a pointer, data() conveniently points to the start of pixels' data
                            pixels.data(),
                            // pitch param wants number of bytes per row of pixels
                            // each pixel is a uint32_t (32 bits / 8 = 4 bytes)
                            // W_WIDTH * 4 gives the row of pixels' length in bytes
                            W_WIDTH*4);

            // copy the texture to the renderer
            SDL_RenderCopy(renderer, 
                            texture,
                            // these are null because this isn't using rect. look to UpdateTexture nullptr comment for reasoning.
                            nullptr,
                            nullptr);
            // display the updated renderer
            SDL_RenderPresent(renderer);
    }
    
    // tearing down renderer and window instances to prevent memory leaks
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}