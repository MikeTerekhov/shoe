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

#endif
