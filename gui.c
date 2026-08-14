#include <stdio.h>

#include "raylib.h"
#include "db.h"

int main(void) {
  InitWindow(800, 450, "Mike's Shoe Tracker");
  SetTargetFPS(60);

  char username[100] = {0};
  int letter_count = 0;
  Rectangle input_box = {260, 220, 280, 40};

  bool logged_in = false;
  char greeting[150] = {0};

  while (!WindowShouldClose()) {
    if (!logged_in) {
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

      if (IsKeyPressed(KEY_ENTER) && letter_count > 0) {
        sqlite3 *db = db_open("shoes.db");
        if (!db) {
          snprintf(greeting, sizeof(greeting), "Could not access the database.");
        } else {
          int is_new = 0;
          int user_id = db_get_or_create_user(db, username, &is_new);
          db_close(db);

          if (user_id < 0) {
            snprintf(greeting, sizeof(greeting), "Something went wrong logging you in.");
          } else if (is_new) {
            snprintf(greeting, sizeof(greeting), "Welcome, %s! You're all set up.", username);
          } else {
            snprintf(greeting, sizeof(greeting), "Hello again, %s!", username);
          }
        }
        logged_in = true;
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Mike's Shoe Tracker", 260, 160, 24, DARKGRAY);

    if (!logged_in) {
      DrawText("Enter username:", 260, 200, 18, DARKGRAY);
      DrawRectangleRec(input_box, LIGHTGRAY);
      DrawRectangleLinesEx(input_box, 2, DARKGRAY);
      DrawText(username, (int)input_box.x + 8, (int)input_box.y + 10, 20, BLACK);
    } else {
      DrawText(greeting, 260, 220, 20, DARKGREEN);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
