#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char username[100];
  printf("Enter username: ");

  fgets(username, sizeof(username), stdin);

  // Remove the newline character that fgets includes
  username[strcspn(username, "\n")] = '\0';

  printf("Hello, %s!\n", username);
  return 0;
}
