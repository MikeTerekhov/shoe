#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
      db_close(db);

      if (user_id < 0) {
        printf("Something went wrong logging you in.\n");
        break;
      }

      if (is_new) {
        printf("Welcome, %s! You're all set up.\n", username);
      } else {
        printf("Hello again, %s!\n", username);
      }
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
