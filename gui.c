#include "raylib.h"

int main(void) {
  InitWindow(800, 450, "Mike's Shoe Tracker");
  SetTargetFPS(60);

  char username[100] = {0};
  int letter_count = 0;
  Rectangle input_box = {260, 220, 280, 40};

  while (!WindowShouldClose()) {
    int key = GetCharPressed();
    while (key > 0) {
      if (key >= 32 && key <= 125 && letter_count < (int)sizeof(username) - 1) {
        username[letter_count] = (char)key;
        letter_count++;
        username[letter_count] = '\0';
      }
      key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && letter_count > 0) {
      letter_count--;
      username[letter_count] = '\0';
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Mike's Shoe Tracker", 260, 160, 24, DARKGRAY);
    DrawText("Enter username:", 260, 200, 18, DARKGRAY);
    DrawRectangleRec(input_box, LIGHTGRAY);
    DrawRectangleLinesEx(input_box, 2, DARKGRAY);
    DrawText(username, (int)input_box.x + 8, (int)input_box.y + 10, 20, BLACK);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
