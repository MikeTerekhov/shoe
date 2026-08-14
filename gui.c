#include "raylib.h"

int main(void) {
  InitWindow(800, 450, "Mike's Shoe Tracker");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Mike's Shoe Tracker", 260, 200, 24, DARKGRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
