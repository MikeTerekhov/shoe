#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
      printf("Hello, %s!\n", username);
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
