#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "db.h"

static void handle_text_input(char *buf, int *len, int max_len) {
  int key = GetCharPressed();
  while (key > 0) {
    if (key >= 32 && key <= 125 && *len < max_len - 1) {
      buf[*len] = (char)key;
      (*len)++;
      buf[*len] = '\0';
    }
    key = GetCharPressed();
  }

  if (IsKeyPressed(KEY_BACKSPACE) && *len > 0) {
    (*len)--;
    buf[*len] = '\0';
  }
}

int main(void) {
  InitWindow(800, 450, "Mike's Shoe Tracker");
  SetTargetFPS(60);

  char username[100] = {0};
  int letter_count = 0;
  Rectangle input_box = {260, 220, 280, 40};
  Rectangle login_button = {260, 275, 120, 40};

  bool logged_in = false;
  int user_id = -1;
  char greeting[150] = {0};

  Shoe shoes[50];
  int shoe_count = 0;

  char brand_input[50] = {0};
  char model_input[50] = {0};
  char size_input[20] = {0};
  int brand_len = 0, model_len = 0, size_len = 0;
  int active_field = -1;
  char add_status[100] = {0};

  Rectangle brand_box = {460, 140, 260, 32};
  Rectangle model_box = {460, 202, 260, 32};
  Rectangle size_box = {460, 264, 100, 32};
  Rectangle add_button = {460, 306, 150, 36};

  while (!WindowShouldClose()) {
    if (!logged_in) {
      handle_text_input(username, &letter_count, sizeof(username));

      bool login_clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                           CheckCollisionPointRec(GetMousePosition(), login_button);

      if ((IsKeyPressed(KEY_ENTER) || login_clicked) && letter_count > 0) {
        sqlite3 *db = db_open("shoes.db");
        if (!db) {
          snprintf(greeting, sizeof(greeting), "Could not access the database.");
        } else {
          int is_new = 0;
          user_id = db_get_or_create_user(db, username, &is_new);

          if (user_id < 0) {
            snprintf(greeting, sizeof(greeting), "Something went wrong logging you in.");
          } else {
            if (is_new) {
              snprintf(greeting, sizeof(greeting), "Welcome, %s! You're all set up.", username);
            } else {
              snprintf(greeting, sizeof(greeting), "Hello again, %s!", username);
            }

            shoe_count = db_get_shoes(db, user_id, shoes, 50);
            if (shoe_count < 0) shoe_count = 0;
          }

          db_close(db);
        }
        logged_in = true;
      }
    } else {
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, brand_box)) active_field = 0;
        else if (CheckCollisionPointRec(mouse, model_box)) active_field = 1;
        else if (CheckCollisionPointRec(mouse, size_box)) active_field = 2;
      }

      if (active_field == 0) {
        handle_text_input(brand_input, &brand_len, sizeof(brand_input));
      } else if (active_field == 1) {
        handle_text_input(model_input, &model_len, sizeof(model_input));
      } else if (active_field == 2) {
        handle_text_input(size_input, &size_len, sizeof(size_input));
      }

      bool add_clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                         CheckCollisionPointRec(GetMousePosition(), add_button);

      if (add_clicked) {
        char *endptr = NULL;
        double size = strtod(size_input, &endptr);

        if (brand_len == 0 || model_len == 0 || endptr == size_input || size <= 0) {
          snprintf(add_status, sizeof(add_status), "Please fill in brand, model, and a valid size.");
        } else {
          sqlite3 *db = db_open("shoes.db");
          if (!db) {
            snprintf(add_status, sizeof(add_status), "Could not access the database.");
          } else {
            if (db_add_shoe(db, user_id, brand_input, model_input, size) < 0) {
              snprintf(add_status, sizeof(add_status), "Could not add that shoe.");
            } else {
              snprintf(add_status, sizeof(add_status), "Added %s %s!", brand_input, model_input);

              brand_input[0] = '\0';
              model_input[0] = '\0';
              size_input[0] = '\0';
              brand_len = model_len = size_len = 0;

              shoe_count = db_get_shoes(db, user_id, shoes, 50);
              if (shoe_count < 0) shoe_count = 0;
            }
            db_close(db);
          }
        }
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Mike's Shoe Tracker", 260, 20, 24, DARKGRAY);

    if (!logged_in) {
      DrawText("Enter username:", 260, 200, 18, DARKGRAY);
      DrawRectangleRec(input_box, LIGHTGRAY);
      DrawRectangleLinesEx(input_box, 2, DARKGRAY);
      DrawText(username, (int)input_box.x + 8, (int)input_box.y + 10, 20, BLACK);

      bool hovered = CheckCollisionPointRec(GetMousePosition(), login_button);
      DrawRectangleRec(login_button, hovered ? GRAY : LIGHTGRAY);
      DrawRectangleLinesEx(login_button, 2, DARKGRAY);
      DrawText("Login", (int)login_button.x + 30, (int)login_button.y + 10, 20, BLACK);
    } else {
      DrawText(greeting, 40, 90, 20, DARKGREEN);
      DrawText("Your shoes:", 40, 130, 18, DARKGRAY);

      if (shoe_count == 0) {
        DrawText("You don't have any shoes yet.", 40, 160, 18, DARKGRAY);
      } else {
        for (int i = 0; i < shoe_count; i++) {
          char line[100];
          snprintf(line, sizeof(line), "%d.) %s %s, size %.1f", i + 1,
                   shoes[i].brand, shoes[i].model, shoes[i].size);
          DrawText(line, 40, 160 + i * 24, 18, DARKGRAY);
        }
      }

      DrawText("Add a shoe:", 460, 90, 18, DARKGRAY);

      DrawText("Brand", (int)brand_box.x, (int)brand_box.y - 20, 14, GRAY);
      DrawRectangleRec(brand_box, active_field == 0 ? LIGHTGRAY : (Color){235, 235, 235, 255});
      DrawRectangleLinesEx(brand_box, 2, DARKGRAY);
      DrawText(brand_input, (int)brand_box.x + 8, (int)brand_box.y + 8, 18, BLACK);

      DrawText("Model", (int)model_box.x, (int)model_box.y - 20, 14, GRAY);
      DrawRectangleRec(model_box, active_field == 1 ? LIGHTGRAY : (Color){235, 235, 235, 255});
      DrawRectangleLinesEx(model_box, 2, DARKGRAY);
      DrawText(model_input, (int)model_box.x + 8, (int)model_box.y + 8, 18, BLACK);

      DrawText("Size", (int)size_box.x, (int)size_box.y - 20, 14, GRAY);
      DrawRectangleRec(size_box, active_field == 2 ? LIGHTGRAY : (Color){235, 235, 235, 255});
      DrawRectangleLinesEx(size_box, 2, DARKGRAY);
      DrawText(size_input, (int)size_box.x + 8, (int)size_box.y + 8, 18, BLACK);

      bool add_hovered = CheckCollisionPointRec(GetMousePosition(), add_button);
      DrawRectangleRec(add_button, add_hovered ? GRAY : LIGHTGRAY);
      DrawRectangleLinesEx(add_button, 2, DARKGRAY);
      DrawText("Add Shoe", (int)add_button.x + 20, (int)add_button.y + 8, 18, BLACK);

      DrawText(add_status, 460, 352, 16, MAROON);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
