#include <iostream>
#include <vector>
#include <cstdint>
#include <SDL.h>
#include "shapes.h"
#include "transform.h"
#include "render.h"
#include <limits>
#include <fstream>
#include <sstream>
// to run this, in root directory: cmake --build build
//                                 ./build/rasterizer

bool dragging = false;
float camera_yaw = 0.0f;
float camera_pitch = 0.0f;


Shape load_obj(std::string filepath) {
    Shape s;
    std::ifstream file(filepath);

    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 2) == "v ") {
            std::stringstream ss(line);
            std::string token;
            
            std::vector<float> float_values;
            while (ss >> token) {
                if (token == "v") continue;
                
                float vector_pos = std::stof(token);
                float_values.push_back(vector_pos);
                
            }
            s.vertices.push_back({float_values[0], float_values[1], float_values[2]});
            
        }
        if (line.substr(0,2) == "f "){
            std::stringstream ss(line);
            std:: string token;

            std::vector<int> indexes;

            while (ss >> token) {
                if (token == "f") continue;
                size_t slash_pos = token.find('/');
                std::string index_str = token.substr(0, slash_pos);

                int index = (std::stoi(index_str)) -1;
                indexes.push_back(index);
            }
            s.triangles.push_back({indexes[0], indexes[1], indexes[2]});            
        }
    }

    float min_x = s.vertices[0].x;
    float min_y= s.vertices[0].y;
    float min_z= s.vertices[0].z;
    float max_x = s.vertices[0].x;
    float max_y = s.vertices[0].y;
    float max_z = s.vertices[0].z;

    for (Vertex vertex : s.vertices) {
        min_x = (vertex.x < min_x) ? vertex.x : min_x;
        max_x = (vertex.x > max_x) ? vertex.x : max_x;
        min_y = (vertex.y < min_y) ? vertex.y : min_y;
        max_y = (vertex.y > max_y) ? vertex.y : max_y;
        min_z = (vertex.z < min_z) ? vertex.z : min_z;
        max_z = (vertex.z > max_z) ? vertex.z : max_z;
    }
    float x_distance = max_x - min_x;
    float y_distance = max_y - min_y;
    float z_distance = max_z - min_z;
    float max_bounding_distance = std::max(std::max(x_distance, y_distance), z_distance);
    float scale_ratio = (W_HEIGHT / max_bounding_distance) * 0.35;

    for (Vertex& vertex : s.vertices) {
        vertex.x *= scale_ratio;
        vertex.y *= scale_ratio;
        vertex.z *= scale_ratio;
    }


    return s;
}

int main() {    
    // Initialize pixel vector (640,000 pixels)
    std::vector<uint32_t> pixels(W_WIDTH * W_HEIGHT, 0xFFFFFFFF); 

    std::vector<float> z_buffer(W_WIDTH* W_HEIGHT, std::numeric_limits<float>::max());

    Vertex light_direction = normalize({0, 1, -1});

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

            
            Shape transformed_box = project_shape(rotate_shape(load_obj("teapot.obj"), camera_pitch, camera_yaw, 0.0f), 750);

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