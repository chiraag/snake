#include "raylib.h"

int main(void) {
  InitWindow(800, 450, "raylib [core] example - basic window");

  // Rectangle location and velocity
  Vector2 rectSize = {20, 20};
  Vector2 rectLoc = {GetScreenWidth() / 2 - 10, GetScreenHeight() / 2 - 10};
  Vector2 dispRectLoc = rectLoc;
  Vector2 rectVel = {2.0f, 0.0f};
  bool isMoving = true;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // Update
    if (IsKeyPressed(KEY_SPACE)) {
      isMoving = !isMoving;
    }

    // Move the rectangle based on its velocity. Quantize location to multiples
    // of rectSize. Loop around the screen.
    if (isMoving) {
      rectLoc.x += rectVel.x;
      rectLoc.y += rectVel.y;
      if (rectLoc.x > GetScreenWidth()) rectLoc.x = 0;
      if (rectLoc.x < 0) rectLoc.x = GetScreenWidth();
      if (rectLoc.y > GetScreenHeight()) rectLoc.y = 0;
      if (rectLoc.y < 0) rectLoc.y = GetScreenHeight();
      dispRectLoc.x = (int)(rectLoc.x / rectSize.x) * rectSize.x;
      dispRectLoc.y = (int)(rectLoc.y / rectSize.y) * rectSize.y;
    }

    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw a 20x20 black rectangle at rectLoc
    DrawRectangleV(dispRectLoc, rectSize, BLACK);

    // Show the pause message and current pause state in the top left corner
    DrawText(isMoving ? "Press [SPACE] to pause" : "Press [SPACE] to unpause",
             10, 10, 20, DARKGRAY);
    // Show the current location of the rectangle in the top right corner
    DrawText(TextFormat("Location: (%03.0f, %03.0f)", rectLoc.x, rectLoc.y),
             GetScreenWidth() - 200, 10, 20, DARKGRAY);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}