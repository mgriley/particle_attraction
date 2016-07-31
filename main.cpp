#include "Simulation.cpp"
#include <iostream>
#include <SDL2/SDL.h>
#include <ctime>
#include <stdio.h>

using namespace std;

int main(int argc, char** argv) {
    int width = 600, height = 600;

    int numParticles = stoi(argv[1]);

    Simulation sim(numParticles, width, height);
    printf("made sim\n");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    } 

    // Get a window
    SDL_Window *win = SDL_CreateWindow("Hello World!", 
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
      width, height, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        cout << "SDL_CreateWindow error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // Get a renderer to draw the window
    // -1 means return the first renderer that meets the specified requirements
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | 
      SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        SDL_DestroyWindow(win);
        cout << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    // Get the window surface
    SDL_Surface* surface = SDL_GetWindowSurface(win);

    // keep a history of past particle positions
    vector<float> hist(width * height, -1.0f);
    float maxVel = 300.0f; // TODO tune

    float timeStep = 1.0f / 60.0f; // run at 60Hz
    clock_t timer = clock(); // init timer
    bool quit = false;
    SDL_Event e;
    while (!quit) {

        // Update at 60Hz
        clock_t now = clock();
        double secsElapsed = (now - timer) / (double) CLOCKS_PER_SEC;
        if (secsElapsed < timeStep) {
          continue;
        }

        // Reset clock
        timer = now;

        // update the simulation
        printf("step\n");
        sim.step(timeStep);

        // Handle events
        while (SDL_PollEvent(&e) != 0) {
          if (e.type == SDL_QUIT) {
            quit = true;
          }
        }

        // clear screen, fill with the draw color
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        // save current particle positions
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        for (int i = 0; i < sim.particles->size(); ++i) {
            Simulation::particle part = sim.particles->at(i);
            v2 p = part.pos;
            int x = (int) p.x;
            int y = (int) p.y;
            if (0 <= x && x < width && 0 <= y && y < height) {
                hist[width * y + x] = max(hist[width * y + x], mag(part.vel));
                SDL_Rect r = {x - 1, y - 1, 3, 3};
                SDL_RenderFillRect(ren, &r);

            }
        }

        // draw entire hist
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                float mag = hist[y * width + x];
                if (mag < 0) continue;
                float scaled = 255 * mag / maxVel;
                SDL_SetRenderDrawColor(ren, scaled, 0, 0, 255);
                SDL_RenderDrawPoint(ren, x, y);
            }
        }
                       
        // NB: SDL rect uses upper-left corner as (x, y)
        /*
        SDL_Rect groundRect = {100, 100, 100, 100};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &groundRect);

        SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
        SDL_RenderDrawLine(ren, 0, 0, 100, 100);
        */

        // update screen
        SDL_RenderPresent(ren);
    }

    // Close
    SDL_FreeSurface(surface);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
