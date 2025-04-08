#include <SDL2/SDL.h>
#include <stdio.h>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }


    SDL_Event event;
    int quit = 0;
    while (!quit) {
      if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = 1;
        }
      }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

