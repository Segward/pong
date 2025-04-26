#include <raylib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  int screenWidth = 800;
  int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "pong [raylib]");
  SetTargetFPS(60); 

  InitAudioDevice();

  Sound soundHit = LoadSound("resources/hit.mp3");
  Music music = LoadMusicStream("resources/wander.mp3"); 
  PlayMusicStream(music);

  int ballRadius = 10;
  float ballSpeed = 200 * 1.0f / 60.0f;
  float playerSpeed = 300 * 1.0f / 60.0f;

  Color darkGray = { 50, 50, 50, 255 };
  Vector2 player1 = { 780, 300 };
  Vector2 player2 = { 0, 300 };
  Vector2 ball = { 400, 300 };
  Vector2 playerSize = { 20, 100 };
  Vector2 ballVelocity = { ballSpeed, ballSpeed };

  int player1Score = 0;
  int player2Score = 0;

  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_W) && player1.y > 0) {
      player2.y -= playerSpeed;
    }

    if (IsKeyDown(KEY_S) && player1.y < screenHeight - playerSize.y) {
      player2.y += playerSpeed;
    }

    if (IsKeyDown(KEY_UP) && player2.y > 0) {
      player1.y -= playerSpeed;
    }

    if (IsKeyDown(KEY_DOWN) && player2.y < screenHeight - playerSize.y) {
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
    }

    if (ball.x > screenWidth) {
      player2Score++;
      ball.x = screenWidth / 2;
      ball.y = screenHeight / 2;
      ballVelocity.x *= -1;
    }

    UpdateMusicStream(music);

    BeginDrawing();
    ClearBackground(darkGray);

    DrawRectangle(player1.x, player1.y, playerSize.x, playerSize.y, BLUE);
    DrawRectangle(player2.x, player2.y, playerSize.x, playerSize.y, RED);
    DrawCircle(ball.x, ball.y, ballRadius, WHITE);

    char scoreText[50];
    sprintf(scoreText, "Player 1: %d - Player 2: %d", player1Score, player2Score);
    DrawText(scoreText, screenWidth / 2 - MeasureText(scoreText, 20) / 2, 10, 20, WHITE);   
 
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
