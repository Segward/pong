#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("font/rockwell.ttf", 24);
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Color color = { 255, 255, 255, 255 };
    uint32_t startTime = SDL_GetTicks();
    int frameCount = 0;
    char buffer[256];

    SDL_Event event;
    int quit = 0;
    while (!quit) {
      if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = 1;
        }
      }
      
      frameCount++;
      if (SDL_GetTicks() - startTime >= 1000) {
        snprintf(buffer, sizeof(buffer), "FPS: %d", frameCount);
        SDL_SetWindowTitle(window, buffer);
        frameCount = 0;
        startTime = SDL_GetTicks();
      }

      SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
      SDL_RenderClear(renderer);

      SDL_Surface* surface = TTF_RenderText_Solid(font, buffer, color);
      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

      int width, height;
      SDL_QueryTexture(texture, NULL, NULL, &width, &height);
      SDL_Rect rect = { 10, 10, width, height };    
      SDL_RenderCopy(renderer, texture, NULL, &rect);

      SDL_DestroyTexture(texture);
      SDL_FreeSurface(surface);
      SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}

