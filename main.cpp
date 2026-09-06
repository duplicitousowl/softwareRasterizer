#include <iostream>
#include <vector>
#include <cstdint>
#include <SDL.h>
// to run this, in root directory: cmake --build build
//                                 ./build/rasterizer

int main() {
    // setting up the window size
    const int V_WIDTH = 600;
    const int V_HEIGHT = 800;
    
    // Initialize pixel vector (480,000 pixels)
    std::vector<uint32_t> pixel_color(V_WIDTH * V_HEIGHT, 0); 

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // credential test

    SDL_CreateWindowAndRenderer(
        V_WIDTH, V_HEIGHT, 0,
        &window, &renderer);

    bool isRunning = true;
    SDL_Event event; 

    while (isRunning) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false; 
            }
        }
    }
  

    std::cout << "Window initialized with dimensions: " << V_WIDTH << "x" << V_HEIGHT << std::endl;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}