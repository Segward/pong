#include <raylib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  int screenWidth = 800;
  int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "pong [raylib]");
  InitAudioDevice();
  SetTargetFPS(60);

  Sound soundHit = LoadSound("resources/hit.mp3");
  Sound soundScore = LoadSound("resources/score.mp3");
  Music music = LoadMusicStream("resources/wander.mp3"); 
  PlayMusicStream(music);

  Shader shader = LoadShader(0, "resources/stars.fs");
  int timeLoc = GetShaderLocation(shader, "iTime");
  int resLoc = GetShaderLocation(shader, "iResolution");
  Vector2 resolution = { (float)screenWidth, (float)screenHeight };

  int ballRadius = 10;
  float ballSpeed = 200 * 1.0f / 60.0f;
  float playerSpeed = 300 * 1.0f / 60.0f;
  Vector2 player1 = { 780, 300 };
  Vector2 player2 = { 0, 300 };
  Vector2 ball = { 400, 300 };
  Vector2 playerSize = { 20, 100 };
  Vector2 ballVelocity = { ballSpeed, ballSpeed };

  int player1Score = 0;
  int player2Score = 0;
  double start = GetTime();

  while (!WindowShouldClose()) {
    float time = GetTime();
    BeginDrawing();
    ClearBackground(BLACK);

    if (time - start <= 3) {
      DrawText("PONG", screenWidth / 2 - MeasureText("PONG", 60) / 2, screenHeight / 2 - 30, 60, WHITE);
      DrawText("Made by Segward", screenWidth / 2 - MeasureText("Made by Segward", 20) / 2, screenHeight / 2 + 30, 20, WHITE);
      EndDrawing();
      continue;
    }

    if (IsKeyDown(KEY_W) && player2.y > 0) {
      player2.y -= playerSpeed;
    }

    if (IsKeyDown(KEY_S) && player2.y < screenHeight - playerSize.y) {
      player2.y += playerSpeed;
    }

    if (IsKeyDown(KEY_UP) && player1.y > 0) {
      player1.y -= playerSpeed;
    }

    if (IsKeyDown(KEY_DOWN) && player1.y < screenHeight - playerSize.y) {
      player1.y += playerSpeed;
    }

    ball.x += ballVelocity.x;
    ball.y += ballVelocity.y;

    if (ball.y < 5 || ball.y > screenHeight - ballRadius / 2) {
      ballVelocity.y *= -1;
    }

    if (ball.x < player2.x + playerSize.x && ball.x > player2.x &&
        ball.y > player2.y && ball.y < player2.y + playerSize.y) {
      ballVelocity.x *= -1;
      PlaySound(soundHit);
    }

    if (ball.x > player1.x - ballRadius && ball.x < player1.x &&
        ball.y > player1.y && ball.y < player1.y + playerSize.y) {
      ballVelocity.x *= -1;
      PlaySound(soundHit);
    }
    
    if (ball.x < 0) {
      player1Score++;
      ball.x = screenWidth / 2;
      ball.y = screenHeight / 2;
      ballVelocity.x *= -1;
      PlaySound(soundScore);
    }

    if (ball.x > screenWidth) {
      player2Score++;
      ball.x = screenWidth / 2;
      ball.y = screenHeight / 2;
      ballVelocity.x *= -1;
      PlaySound(soundScore);
    }

    UpdateMusicStream(music);

    BeginShaderMode(shader);
    SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, resLoc, &resolution, SHADER_UNIFORM_VEC2);
    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
    EndShaderMode();

    DrawRectangle(player1.x, player1.y, playerSize.x, playerSize.y, BLUE);
    DrawRectangle(player2.x, player2.y, playerSize.x, playerSize.y, RED);
    DrawCircle(ball.x, ball.y, ballRadius, WHITE);

    DrawText(TextFormat("%d", player1Score), screenWidth / 2 - 50, 20, 40, BLUE);
    DrawText(TextFormat("%d", player2Score), screenWidth / 2 + 20, 20, 40, RED);
 
    EndDrawing();
  }

  UnloadShader(shader);
  CloseWindow();
  return 0;
}
