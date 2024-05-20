#include <stdlib.h>
#include <time.h>

#include "raylib.h"

void MoveRect(Vector2 *loc, const Vector2 *vel, const Vector2 *bound) {
  loc->x += vel->x;
  loc->y += vel->y;
  if (loc->x > bound->x) loc->x = 0;
  if (loc->x < 0) loc->x = bound->x;
  if (loc->y > bound->y) loc->y = 0;
  if (loc->y < 0) loc->y = bound->y;
}

void QuantizeLoc(Vector2 *outLoc, const Vector2 *inLoc, const Vector2 *size) {
  outLoc->x = (int)(inLoc->x / size->x) * size->x;
  outLoc->y = (int)(inLoc->y / size->y) * size->y;
}

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

void UpdateDirection(Direction *dir) {
  if (IsKeyPressed(KEY_UP) && *dir != DOWN) {
    *dir = UP;
  } else if (IsKeyPressed(KEY_DOWN) && *dir != UP) {
    *dir = DOWN;
  } else if (IsKeyPressed(KEY_LEFT) && *dir != RIGHT) {
    *dir = LEFT;
  } else if (IsKeyPressed(KEY_RIGHT) && *dir != LEFT) {
    *dir = RIGHT;
  }
}

void SetVelocity(Vector2 *vel, const Direction dir, const float speed) {
  switch (dir) {
    case UP:
      vel->x = 0;
      vel->y = -speed;
      break;
    case DOWN:
      vel->x = 0;
      vel->y = speed;
      break;
    case LEFT:
      vel->x = -speed;
      vel->y = 0;
      break;
    case RIGHT:
      vel->x = speed;
      vel->y = 0;
      break;
  }
}

void GetRandomLoc(Vector2 *loc, const Vector2 *bound, const Vector2 *size) {
  loc->x = GetRandomValue(0, bound->x);
  loc->y = GetRandomValue(0, bound->y);
  QuantizeLoc(loc, loc, size);
}

int main(void) {
  const Vector2 screenSize = {800, 460};
  InitWindow(screenSize.x, screenSize.y, "Snake");

  // If DEBUG is defined, use a fixed seed else use the current time
#ifdef DEBUG
  int seed = 0;
#else
  srand(time(NULL));
  int seed = rand();
#endif
  SetRandomSeed(seed);

  // Rectangle location and velocity
  Vector2 rectSize = {20, 20};
  Vector2 rectLoc = {GetScreenWidth() / 2 - 10, GetScreenHeight() / 2 - 10};
  Vector2 dispRectLoc = rectLoc;
  float rectSpeed = 2.0f;
  float rectAccel = 1.05f;
  Direction dir = RIGHT;
  Vector2 rectVel = {rectSpeed, 0.0f};
  bool isMoving = true;
  Vector2 appleLoc;
  GetRandomLoc(&appleLoc, &screenSize, &rectSize);

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // Update
    // Pause movement for debug
    if (IsKeyPressed(KEY_SPACE)) {
      isMoving = !isMoving;
    }

    // Update velocity based on arrow keys (snake rules i.e. no reversing)
    UpdateDirection(&dir);
    SetVelocity(&rectVel, dir, rectSpeed);

    // Move the rectangle based on its velocity. Quantize location to multiples
    // of rectSize. Loop around the screen.
    if (isMoving) {
      MoveRect(&rectLoc, &rectVel, &screenSize);
      QuantizeLoc(&dispRectLoc, &rectLoc, &rectSize);

      // Check collision with apple
      // On collision, move apple to new random location
      // Increase velocity by 5%
      if (dispRectLoc.x == appleLoc.x && dispRectLoc.y == appleLoc.y) {
        GetRandomLoc(&appleLoc, &screenSize, &rectSize);
        rectSpeed *= rectAccel;
      }
    }

    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw a 20x20 black rectangle at rectLoc
    DrawRectangleV(dispRectLoc, rectSize, BLACK);

    // Draw a 20x20 dark red rectangle at appleLoc
    DrawRectangleV(appleLoc, rectSize, MAROON);

    // Show the pause message and current pause state in the top left corner
    DrawText(isMoving ? "Press [SPACE] to pause" : "Press [SPACE] to unpause",
             10, 10, 20, DARKGRAY);
    // Show the current location of the rectangle in the top right corner
    // DrawText(TextFormat("Location: (%03.0f, %03.0f)", rectLoc.x, rectLoc.y),
    //          GetScreenWidth() - 200, 10, 20, DARKGRAY);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}