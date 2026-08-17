#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// this is a test

#include "db.h"

int main() {
  printf("-----------------------------------------\n");
  printf("Welcome to Mike's Shoe Tracker App\n");
  printf("-----------------------------------------\n");
  printf("Here are the current options for what is available: \n");
  printf("a.) Login \n");
  printf("q.) Quit \n");
  printf("-----------------------------------------\n");

  char selection;

  printf("Please enter an option:  \n");
  scanf("%c", &selection);

  // this is for clearing the input buffer for scanf
  while(getchar() != '\n');

  switch(selection) {
    case 'A' : case 'a' : {
      char username[100];
      printf("Enter username: ");
      fgets(username, sizeof(username), stdin);
      // Remove the newline character that fgets includes
      username[strcspn(username, "\n")] = '\0';

      sqlite3 *db = db_open("shoes.db");
      if (!db) {
        printf("Could not access the database.\n");
        break;
      }

      int is_new = 0;
      int user_id = db_get_or_create_user(db, username, &is_new);

      if (user_id < 0) {
        printf("Something went wrong logging you in.\n");
        db_close(db);
        break;
      }

      if (is_new) {
        printf("Welcome, %s! You're all set up.\n", username);
      } else {
        printf("Hello again, %s!\n", username);
      }

      char shoe_selection;
      do {
        printf("-----------------------------------------\n");
        printf("s.) Add a shoe \n");
        printf("l.) List my shoes \n");
        printf("b.) Back / Quit \n");
        printf("-----------------------------------------\n");
        printf("Please enter an option:  \n");
        scanf("%c", &shoe_selection);
        while (getchar() != '\n');

        switch (shoe_selection) {
          case 'S': case 's': {
            char brand[50], model[50];
            double size;

            printf("Brand: ");
            fgets(brand, sizeof(brand), stdin);
            brand[strcspn(brand, "\n")] = '\0';

            printf("Model: ");
            fgets(model, sizeof(model), stdin);
            model[strcspn(model, "\n")] = '\0';

            printf("Size: ");
            scanf("%lf", &size);
            while (getchar() != '\n');

            if (db_add_shoe(db, user_id, brand, model, size) < 0) {
              printf("Could not add that shoe.\n");
            } else {
              printf("Added %s %s, size %.1f!\n", brand, model, size);
            }
            break;
          }
          case 'L': case 'l': {
            int count = db_list_shoes(db, user_id);
            if (count == 0) {
              printf("You don't have any shoes yet.\n");
            }
            break;
          }
          case 'B': case 'b':
            printf("Goodbye, %s!\n", username);
            break;
          default:
            printf("Sorry, that is not an option!\n");
        }
      } while (shoe_selection != 'B' && shoe_selection != 'b');

      db_close(db);
      break;
    }
    case 'Q' : case 'q':
      printf("Goodbye!\n");
      break;
    default:
      printf("Sorry, that is not an option!\n");
  }


  return 0;
}
