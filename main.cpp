#include <iostream>
#include <vector>
#include <cstdint>
#include <SDL.h>
#include "shapes.h"
#include "transform.h"
#include "render.h"
#include <limits>
// to run this, in root directory: cmake --build build
//                                 ./build/rasterizer

float theta = -0.1f;
bool dragging = false;
float camera_yaw = 0.0f;
float camera_pitch = 0.0f;


// for testing draw_shape
Shape make_test_box() {
    Shape s;
    s.vertices = {
        {-100, 100, -100}, // back top left 0
        {-100, -100, -100}, // back bottom left 1
        {100, -100, -100}, // back bottom right 2
        {100, 100, -100}, // back top right 3
        {-100, 100, 100}, // front top left 4
        {-100, -100, 100}, // front bottom left 5
        {100, -100, 100}, // front bottom right 6
        {100, 100, 100} // front top right 7
    };
    s.edges = {
        {0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6}, {6,7},{7,4},{0,4},{1,5},{2,6},{3,7}
    };
    s.triangles = {
    {0,3,2}, {0,2,1}, // back
    {4,5,6}, {4,6,7}, // front
    {2,3,7}, {2,7,6}, // right
    {0,1,5}, {0,5,4}, // left
    {0,4,7}, {0,7,3}, // top
    {1,2,6}, {1,6,5}  // bottom
};
    return s;
}

Shape make_test_triangle() {
    Shape t;
    t.vertices = {
        {0,0},
        {100, 0},
        {0,100}
    };
    t.edges = {
        {0,1}, {1,2}, {2,0}
    };
    t.triangles = {{0,1,2}};

    return t;
}

int main() {    
    // Initialize pixel vector (640,000 pixels)
    std::vector<uint32_t> pixels(W_WIDTH * W_HEIGHT, 0xFFFFFFFF); 

    std::vector<float> z_buffer(W_WIDTH* W_HEIGHT, std::numeric_limits<float>::max());

    Vertex light_direction = normalize({0, 0, -1});

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
    Shape test_triangle = make_test_triangle();

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

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                dragging = true;
}
            else if (event.type == SDL_MOUSEBUTTONUP) {
                dragging = false;
}
            else if (event.type == SDL_MOUSEMOTION) {
                if (dragging) {
                    camera_yaw -= event.motion.xrel * 0.005f;
                    camera_pitch -= event.motion.yrel * 0.005f;
    }
}
        }   

            uint32_t black = 0xFF000000;
            uint32_t white = 0xFFFFFFFF;
            uint32_t gray = 0xFF969696;
            // reset vector at beginning of every loop so there's no lingering line data
            std::fill(pixels.begin(), pixels.end(), white);
            std::fill(z_buffer.begin(), z_buffer.end(), std::numeric_limits<float>::max());

            

            theta -= 0.0015f;
            
            Shape transformed_box = project_shape(rotate_shape(test_box, camera_pitch, camera_yaw, 0.0f), 750);

            draw_shape(transformed_box, black, pixels);
            fill_shape(transformed_box, gray, pixels, z_buffer, light_direction);

            // fill_shape(test_triangle, white, pixels);
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