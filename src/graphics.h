#include <SDL2/SDL.h>
#include <stdio.h>

SDL_Window* createWindow(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    return renderer;
}

void clearScreen(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
}

void drawRectangle(SDL_Renderer* renderer, int x, int y, int width, int height, int red, int green, int blue) {
    SDL_Rect rect = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void cleanup(SDL_Window* window, SDL_Renderer* renderer) {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}
