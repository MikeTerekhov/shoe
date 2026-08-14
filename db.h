#ifndef DB_H
#define DB_H

#include <sqlite3.h>

// Opens (creating if needed) the database at path and ensures the users
// table exists. Returns NULL on failure.
sqlite3 *db_open(const char *path);

void db_close(sqlite3 *db);

// Looks up username in the users table, inserting it if not present.
// Sets *is_new to 1 if the user was just created, 0 if it already existed.
// Returns the user's id, or -1 on error.
int db_get_or_create_user(sqlite3 *db, const char *username, int *is_new);

// Adds a shoe owned by user_id. Returns the new shoe's id, or -1 on error.
int db_add_shoe(sqlite3 *db, int user_id, const char *brand,
                 const char *model, double size);

// Prints every shoe owned by user_id to stdout. Returns the number of shoes
// printed, or -1 on error.
int db_list_shoes(sqlite3 *db, int user_id);

#endif
