#include <raylib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  InitWindow(800, 600, "raylib");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText("Hello, World!", 190, 200, 20, LIGHTGRAY);
    DrawText("Press ESC to exit", 190, 220, 20, LIGHTGRAY);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
