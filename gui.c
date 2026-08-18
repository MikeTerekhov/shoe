#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "db.h"

#define LIST_TOP 160
#define ROW_H 46

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

static Color color_for_tag(const char *tag) {
  if (strcmp(tag, "Easy Day") == 0) return (Color){70, 120, 200, 255};
  if (strcmp(tag, "Race Shoe") == 0) return (Color){200, 60, 60, 255};
  if (strcmp(tag, "Trail Shoe") == 0) return (Color){50, 140, 70, 255};
  return GRAY;
}

int main(void) {
  InitWindow(800, 560, "Mike's Shoe Tracker");
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
  Rectangle logout_button = {690, 16, 90, 30};

  int selected_shoe_index = -1;
  char miles_input[20] = {0};
  int miles_len = 0;
  char miles_status[100] = {0};

  Rectangle miles_box = {40, 400, 100, 32};
  Rectangle add_miles_button = {150, 400, 130, 32};

  char tag_status[100] = {0};
  const char *tag_options[] = {"Easy Day", "Race Shoe", "Trail Shoe"};
  Rectangle tag_buttons[3] = {
      {40, 470, 110, 32},
      {160, 470, 110, 32},
      {280, 470, 110, 32},
  };
  Color tag_colors[3] = {
      (Color){190, 220, 255, 255},  // Easy Day: blue
      (Color){255, 190, 190, 255},  // Race Shoe: red
      (Color){190, 235, 195, 255},  // Trail Shoe: green
  };

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
      bool logout_clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                            CheckCollisionPointRec(GetMousePosition(), logout_button);

      if (logout_clicked) {
        logged_in = false;
        user_id = -1;
        username[0] = '\0';
        letter_count = 0;
        greeting[0] = '\0';
        shoe_count = 0;
        brand_input[0] = '\0';
        model_input[0] = '\0';
        size_input[0] = '\0';
        brand_len = model_len = size_len = 0;
        active_field = -1;
        add_status[0] = '\0';
        selected_shoe_index = -1;
        miles_input[0] = '\0';
        miles_len = 0;
        miles_status[0] = '\0';
        tag_status[0] = '\0';
        continue;
      }

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();

        for (int i = 0; i < shoe_count; i++) {
          Rectangle delete_box = {360, LIST_TOP + i * ROW_H - 2, 20, 20};
          if (CheckCollisionPointRec(mouse, delete_box)) {
            sqlite3 *db = db_open("shoes.db");
            if (db) {
              db_delete_shoe(db, user_id, shoes[i].id);
              shoe_count = db_get_shoes(db, user_id, shoes, 50);
              if (shoe_count < 0) shoe_count = 0;
              db_close(db);
            }
            selected_shoe_index = -1;
            break;
          }
        }

        for (int i = 0; i < shoe_count; i++) {
          Rectangle row_box = {40, LIST_TOP + i * ROW_H - 2, 310, ROW_H - 4};
          if (CheckCollisionPointRec(mouse, row_box)) {
            selected_shoe_index = i;
            miles_input[0] = '\0';
            miles_len = 0;
            miles_status[0] = '\0';
            tag_status[0] = '\0';
            break;
          }
        }

        if (CheckCollisionPointRec(mouse, brand_box)) active_field = 0;
        else if (CheckCollisionPointRec(mouse, model_box)) active_field = 1;
        else if (CheckCollisionPointRec(mouse, size_box)) active_field = 2;
        else if (CheckCollisionPointRec(mouse, miles_box)) active_field = 3;
      }

      if (active_field == 0) {
        handle_text_input(brand_input, &brand_len, sizeof(brand_input));
      } else if (active_field == 1) {
        handle_text_input(model_input, &model_len, sizeof(model_input));
      } else if (active_field == 2) {
        handle_text_input(size_input, &size_len, sizeof(size_input));
      } else if (active_field == 3) {
        handle_text_input(miles_input, &miles_len, sizeof(miles_input));
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

      bool add_miles_clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                               CheckCollisionPointRec(GetMousePosition(), add_miles_button);

      if (add_miles_clicked) {
        if (selected_shoe_index < 0 || selected_shoe_index >= shoe_count) {
          snprintf(miles_status, sizeof(miles_status), "Click a shoe above first.");
        } else {
          char *endptr = NULL;
          double miles = strtod(miles_input, &endptr);

          if (endptr == miles_input || miles <= 0) {
            snprintf(miles_status, sizeof(miles_status), "Enter a valid number of miles.");
          } else {
            sqlite3 *db = db_open("shoes.db");
            if (!db) {
              snprintf(miles_status, sizeof(miles_status), "Could not access the database.");
            } else {
              int shoe_id = shoes[selected_shoe_index].id;
              if (db_add_miles(db, user_id, shoe_id, miles) < 0) {
                snprintf(miles_status, sizeof(miles_status), "Could not add miles.");
              } else {
                snprintf(miles_status, sizeof(miles_status), "Added %.1f miles!", miles);
                miles_input[0] = '\0';
                miles_len = 0;

                shoe_count = db_get_shoes(db, user_id, shoes, 50);
                if (shoe_count < 0) shoe_count = 0;
              }
              db_close(db);
            }
          }
        }
      }

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();

        for (int i = 0; i < 3; i++) {
          if (CheckCollisionPointRec(mouse, tag_buttons[i])) {
            if (selected_shoe_index < 0 || selected_shoe_index >= shoe_count) {
              snprintf(tag_status, sizeof(tag_status), "Click a shoe above first.");
            } else {
              sqlite3 *db = db_open("shoes.db");
              if (!db) {
                snprintf(tag_status, sizeof(tag_status), "Could not access the database.");
              } else {
                int shoe_id = shoes[selected_shoe_index].id;
                if (db_set_shoe_tag(db, user_id, shoe_id, tag_options[i]) < 0) {
                  snprintf(tag_status, sizeof(tag_status), "Could not set tag.");
                } else {
                  snprintf(tag_status, sizeof(tag_status), "Tagged as %s!", tag_options[i]);
                  shoe_count = db_get_shoes(db, user_id, shoes, 50);
                  if (shoe_count < 0) shoe_count = 0;
                }
                db_close(db);
              }
            }
            break;
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
      bool logout_hovered = CheckCollisionPointRec(GetMousePosition(), logout_button);
      DrawRectangleRec(logout_button, logout_hovered ? GRAY : LIGHTGRAY);
      DrawRectangleLinesEx(logout_button, 2, DARKGRAY);
      DrawText("Logout", (int)logout_button.x + 12, (int)logout_button.y + 6, 18, BLACK);

      DrawText(greeting, 40, 90, 20, DARKGREEN);
      DrawText("Your shoes:", 40, 130, 18, DARKGRAY);

      if (shoe_count == 0) {
        DrawText("You don't have any shoes yet.", 40, 160, 18, DARKGRAY);
      } else {
        for (int i = 0; i < shoe_count; i++) {
          int row_y = LIST_TOP + i * ROW_H;
          Rectangle row_box = {40, row_y - 2, 310, ROW_H - 4};
          if (i == selected_shoe_index) {
            DrawRectangleRec(row_box, (Color){220, 235, 255, 255});
          }

          char line[80];
          snprintf(line, sizeof(line), "%d.) %s %s, size %.1f", i + 1,
                   shoes[i].brand, shoes[i].model, shoes[i].size);
          DrawText(line, 40, row_y, 18, DARKGRAY);

          char miles_line[40];
          snprintf(miles_line, sizeof(miles_line), "%.1f mi", shoes[i].miles);
          DrawText(miles_line, 60, row_y + 22, 14, GRAY);

          if (shoes[i].tag[0] != '\0') {
            int offset = MeasureText(miles_line, 14) + 10;
            DrawText(shoes[i].tag, 60 + offset, row_y + 22, 14, color_for_tag(shoes[i].tag));
          }

          Rectangle delete_box = {360, row_y - 2, 20, 20};
          bool delete_hovered = CheckCollisionPointRec(GetMousePosition(), delete_box);
          DrawRectangleRec(delete_box, delete_hovered ? (Color){255, 200, 200, 255}
                                                       : (Color){235, 235, 235, 255});
          DrawRectangleLinesEx(delete_box, 1, MAROON);
          DrawText("x", (int)delete_box.x + 6, (int)delete_box.y + 2, 16, MAROON);
        }
      }

      if (selected_shoe_index >= 0 && selected_shoe_index < shoe_count) {
        char sel_label[100];
        snprintf(sel_label, sizeof(sel_label), "Add miles to %s %s:",
                 shoes[selected_shoe_index].brand, shoes[selected_shoe_index].model);
        DrawText(sel_label, 40, 375, 16, DARKGRAY);
      } else {
        DrawText("Click a shoe above to add miles.", 40, 375, 16, GRAY);
      }

      DrawRectangleRec(miles_box, active_field == 3 ? LIGHTGRAY : (Color){235, 235, 235, 255});
      DrawRectangleLinesEx(miles_box, 2, DARKGRAY);
      DrawText(miles_input, (int)miles_box.x + 8, (int)miles_box.y + 8, 18, BLACK);

      bool add_miles_hovered = CheckCollisionPointRec(GetMousePosition(), add_miles_button);
      DrawRectangleRec(add_miles_button, add_miles_hovered ? GRAY : LIGHTGRAY);
      DrawRectangleLinesEx(add_miles_button, 2, DARKGRAY);
      DrawText("Add Miles", (int)add_miles_button.x + 15, (int)add_miles_button.y + 8, 16, BLACK);

      DrawText(miles_status, 40, 442, 14, MAROON);

      for (int i = 0; i < 3; i++) {
        bool tag_hovered = CheckCollisionPointRec(GetMousePosition(), tag_buttons[i]);
        Color fill = tag_hovered ? ColorBrightness(tag_colors[i], -0.2f) : tag_colors[i];
        DrawRectangleRec(tag_buttons[i], fill);
        DrawRectangleLinesEx(tag_buttons[i], 2, DARKGRAY);
        DrawText(tag_options[i], (int)tag_buttons[i].x + 8, (int)tag_buttons[i].y + 8, 14, BLACK);
      }

      DrawText(tag_status, 40, 512, 14, MAROON);

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
