#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"

typedef struct Snake {
  int length;
  Vector2 *loc;
} Snake;

void Copy(Vector2 *dest, const Vector2 *src) {
  dest->x = src->x;
  dest->y = src->y;
}

void MoveRect(Vector2 *loc, const Vector2 *vel, const Vector2 *bound) {
  loc->x += vel->x;
  loc->y += vel->y;
  if (loc->x >= bound->x) loc->x = 0;
  if (loc->x < 0) loc->x = bound->x - 1;
  if (loc->y >= bound->y) loc->y = 0;
  if (loc->y < 0) loc->y = bound->y - 1;
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
  loc->x = GetRandomValue(0, bound->x - 1);
  loc->y = GetRandomValue(0, bound->y - 1);
  QuantizeLoc(loc, loc, size);
}

void SnakeExtend(Snake *snake, const Vector2 *loc) {
  for (int i = snake->length; i > 0; i--) {
    Copy(&snake->loc[i], &snake->loc[i - 1]);
  }
  Copy(&snake->loc[0], loc);
  (snake->length)++;
}

bool CheckCollisionAndUpdate(Snake *snake, const Vector2 *loc) {
  // If loc is same as head, no motion since last step
  if (Vector2Equals(snake->loc[0], *loc)) {
    return false;
  }

  // Else check all other parts of the snake
  for (int i = 1; i < snake->length; i++) {
    if (Vector2Equals(snake->loc[i], *loc)) {
      return true;
#ifdef DEBUG
      TraceLog(LOG_INFO, "[C] Collision at (%d, %d)", (int)loc->x, (int)loc->y);
#endif
    }
  }

  // Update the snake location
  for (int i = snake->length - 1; i > 0; i--) {
    Copy(&snake->loc[i], &snake->loc[i - 1]);
  }
  Copy(&snake->loc[0], loc);
  return false;
}

void DrawSnake(Snake *snake, const Vector2 *size) {
  for (int i = 0; i < snake->length; i++) {
    DrawRectangleV(snake->loc[i], *size, BLACK);
  }
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
  Vector2 rectLoc = {screenSize.x / 2 - rectSize.x / 2,
                     screenSize.y / 2 - rectSize.y / 2};
  Vector2 nextLoc = rectLoc;
  float rectSpeed = 2.0f;
  float rectAccel = 1.05f;
  Direction dir = RIGHT;
  Vector2 rectVel = {rectSpeed, 0.0f};
  bool isMoving = true;
  bool gameOver = false;

  // Statically allocate max length of the snake
  int maxSnakeLength =
      (screenSize.x / rectSize.x) * (screenSize.y / rectSize.y);
  Vector2 snakeLoc[maxSnakeLength];

  Snake snake = {.length = 1, .loc = snakeLoc};
  Copy(&snake.loc[0], &rectLoc);

  Vector2 appleLoc;
  GetRandomLoc(&appleLoc, &screenSize, &rectSize);
  int textWidth;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // Update
    if (!gameOver) {
      // Pause movement for debug
      if (IsKeyPressed(KEY_SPACE)) {
        isMoving = !isMoving;
      }

      // Update velocity based on arrow keys (snake rules i.e. no reversing)
      UpdateDirection(&dir);
      SetVelocity(&rectVel, dir, rectSpeed);

      // Move the rectangle based on its velocity. Quantize location to
      // multiples of rectSize. Loop around the screen.
      if (isMoving) {
        MoveRect(&rectLoc, &rectVel, &screenSize);
        QuantizeLoc(&nextLoc, &rectLoc, &rectSize);

        // Check collision with apple
        // On collision, move apple to new random location
        // Increase velocity by 5%
        if (Vector2Equals(nextLoc, appleLoc)) {
          SnakeExtend(&snake, &nextLoc);
          GetRandomLoc(&appleLoc, &screenSize, &rectSize);
          rectSpeed *= rectAccel;
#ifdef DEBUG
          TraceLog(LOG_INFO, "[E] Next Apple: (%d, %d)", (int)appleLoc.x,
                   (int)appleLoc.y);
#endif
        } else {
          // Check collision with self
          if (CheckCollisionAndUpdate(&snake, &nextLoc)) {
            gameOver = true;
          }
        }
      }
    }

    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawSnake(&snake, &rectSize);

    // Draw a 20x20 dark red rectangle at appleLoc
    DrawRectangleV(appleLoc, rectSize, MAROON);

    // Show the pause message and current pause state in the bottom left corner
#ifdef DEBUG
    DrawText(isMoving ? "Press [SPACE] to pause" : "Press [SPACE] to unpause",
             10, GetScreenHeight() - 30, 20, LIGHTGRAY);
    DrawText(TextFormat("Location: (%03.0f, %03.0f)", nextLoc.x, nextLoc.y),
             GetScreenWidth() - 200, GetScreenHeight() - 30, 20, LIGHTGRAY);
    DrawText(TextFormat("FPS: %02d", GetFPS()), 10, 10, 20, LIGHTGRAY);
#endif
    // Show appleCount as score in the top right corner
    DrawText(TextFormat("Score: %03d", snake.length - 1),
             GetScreenWidth() - 120, 10, 20, DARKGRAY);

    if (gameOver) {
      // Draw transparent rectangle and text for game over
      DrawRectangle(0, 0, screenSize.x, screenSize.y, Fade(RAYWHITE, 0.7f));
      textWidth = MeasureText("Game Over", 30);
      DrawText("Game Over", screenSize.x / 2 - textWidth / 2, screenSize.y / 2,
               30, MAROON);
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}