#include <iostream>
#include <vector>
#include <cstdint>
#include <SDL.h>
// to run this, in root directory: cmake --build build
//                                 ./build/rasterizer

// setting up the window size
    const int W_WIDTH = 600;
    const int W_HEIGHT = 800;

// make helper struct for 2d screen coordinates
struct Point {
    int x;
    int y;
};

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

    // instantiate the error. starting with dx - dy makes y our floor and x our ceiling
    int err = dx - dy;

    while (true) {
        // place a pixel at the target pixel at the start of every loop
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

int main() {    
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
        }   

            Point center = c_to_screen(0,0);

            uint32_t red = 0xFF0000FF;
            uint32_t green = 0xFF00FF00;

            Point p1 = c_to_screen(-250,-250);
            Point p2 = c_to_screen(250, -250);
            Point p3 = c_to_screen(250, 250);
            Point p4 = c_to_screen(-250, 250);
            Point sb1 = c_to_screen(-100,-100);
            Point sb2 = c_to_screen(100, -100);
            Point sb3 = c_to_screen(100, 100);
            Point sb4 = c_to_screen(-100, 100);

            draw_line(p1.x, p1.y, p2.x, p2.y, red, pixels);
            draw_line(p2.x, p2.y, p3.x, p3.y, red, pixels);
            draw_line(p3.x, p3.y, p4.x, p4.y, red, pixels);
            draw_line(p4.x, p4.y, p1.x, p1.y, red, pixels);
            draw_line(sb1.x, sb1.y, sb2.x, sb2.y, green, pixels);
            draw_line(sb2.x, sb2.y, sb3.x, sb3.y, green, pixels);
            draw_line(sb3.x, sb3.y, sb4.x, sb4.y, green, pixels);
            draw_line(sb4.x, sb4.y, sb1.x, sb1.y, green, pixels);
            draw_line(p1.x, p1.y, sb1.x, sb1.y, red, pixels);
            draw_line(p2.x, p2.y, sb2.x, sb2.y, red, pixels);
            draw_line(p3.x, p3.y, sb3.x, sb3.y, red, pixels);
            draw_line(p4.x, p4.y, sb4.x, sb4.y, red, pixels);

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