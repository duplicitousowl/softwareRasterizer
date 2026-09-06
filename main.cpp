#include <iostream>
#include <vector>
#include <cstdint>
#include <SDL.h>
// to run this, in root directory: cmake --build build
//                                 ./build/rasterizer

int main() {
    // setting up the window size
    const int W_WIDTH = 600;
    const int W_HEIGHT = 800;
    
    // Initialize pixel vector (480,000 pixels)
    std::vector<uint32_t> pixels(W_WIDTH * W_HEIGHT, 0); 

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
                                    // this is for updating pixel data with custom data, our vector. enum of 1. alternatives are SDL_TEXTUREACCESS_STATIC (enum of 0) for static images, and SDL_TEXTUREACCESS_TARGET (enum of 2) for using SDL tools for pixel data
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
        }
            // updates SDL texture with our vector data. 
            SDL_UpdateTexture(texture,
                            // null since we aren't using rect. we are updating entire texture/renderer so dont need to specify a region
                            nullptr,
                            // requires a pointer, data() conveniently points to the start of our data
                            pixels.data(), 
                            W_WIDTH*4);
                            
            // clear the renderer in anticipation of filling it with our updated texture
            SDL_RenderClear(renderer);

            // copy the texture to the renderer
            SDL_RenderCopy(renderer, 
                            texture,
                            // these are null because we are not using rect 
                            nullptr,
                            nullptr);
            
            // display our updated renderer
            SDL_RenderPresent(renderer);
    }
    
    // tearing down renderer and window instances to prevent memory leaks
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}