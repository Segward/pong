#include "graphics.h"

int main() {
    SDL_Window* window = createWindow("SDL Window", 800, 600);
    if (!window) {
        return 1;
    }

    SDL_Renderer* renderer = createRenderer(window);
    if (!renderer) {
        cleanup(window, NULL);
        return 1;
    }

    clearScreen(renderer);
    drawRectangle(renderer, 100, 100, 200, 150, 255, 0, 0);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    int quit = 0;
    while (!quit) {
      if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = 1;
        }
      }
    }
    
    cleanup(window, renderer);
    return 0;
}

