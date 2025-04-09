#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_SHOWN);
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

    TTF_Font *font = TTF_OpenFont("font/rockwell.ttf", 20);
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    float posX = 300.f;
    float posY = 200.f;
    float velX = 0.f;
    float velY = 0.f;
    float speed = 500.f;

    SDL_Color color = { 255, 255, 255, 255 };
    uint32_t startTime = SDL_GetTicks();
    uint32_t deltaFrameCount = 0;
    int frameCount = 0;
    char buffer[256];

    SDL_Event event;
    int quit = 0;
    while (!quit) {
      if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          quit = 1;
        }

        if (event.type == SDL_KEYDOWN) {
          switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
              quit = 1;
              break;
            case SDLK_UP:
              velY = -speed;
              break;
            case SDLK_DOWN:
              velY = speed;
              break;
            case SDLK_LEFT:
              velX = -speed;
              break;
            case SDLK_RIGHT:
              velX = speed;
              break;
          }
        }

        if (event.type == SDL_KEYUP) {
          switch (event.key.keysym.sym) {
            case SDLK_UP:
            case SDLK_DOWN:
              velY = 0.f;
              break;
            case SDLK_LEFT:
            case SDLK_RIGHT:
              velX = 0.f;
              break;
          }
        }
      }
      
      frameCount++;
      if (SDL_GetTicks() - startTime >= 1000) {
        snprintf(buffer, sizeof(buffer), "FPS: %d", frameCount);
        deltaFrameCount = frameCount;
        frameCount = 0;
        startTime = SDL_GetTicks();
      }

      SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
      SDL_RenderClear(renderer);

      SDL_Surface* surface = TTF_RenderText_Blended(font, buffer, color);
      SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

      int width, height;
      SDL_QueryTexture(texture, NULL, NULL, &width, &height);
      SDL_Rect rect = { 10, 10, width, height };    
      SDL_RenderCopy(renderer, texture, NULL, &rect);

      float deltaTime = 1.f / deltaFrameCount;
      float deltaX = velX * deltaTime;
      float deltaY = velY * deltaTime;
      
      if (isnan(deltaX) || isnan(deltaY)) {
        deltaX = 0.f;
        deltaY = 0.f;
      }

      posX += deltaX;
      posY += deltaY;

      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      SDL_Rect playerRect = {posX,posY, 50, 50};
      SDL_RenderFillRect(renderer, &playerRect);

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

