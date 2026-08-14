#include "db.h"

#include <stdio.h>

sqlite3 *db_open(const char *path) {
  sqlite3 *db;
  if (sqlite3_open(path, &db) != SQLITE_OK) {
    fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return NULL;
  }

  const char *sql =
      "CREATE TABLE IF NOT EXISTS users ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "username TEXT NOT NULL UNIQUE"
      ");"
      "CREATE TABLE IF NOT EXISTS shoes ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "user_id INTEGER NOT NULL, "
      "brand TEXT NOT NULL, "
      "model TEXT NOT NULL, "
      "size REAL NOT NULL, "
      "FOREIGN KEY(user_id) REFERENCES users(id)"
      ");";

  char *err_msg = NULL;
  if (sqlite3_exec(db, sql, NULL, NULL, &err_msg) != SQLITE_OK) {
    fprintf(stderr, "Failed to create tables: %s\n", err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return NULL;
  }

  return db;
}

void db_close(sqlite3 *db) {
  if (db) {
    sqlite3_close(db);
  }
}

int db_get_or_create_user(sqlite3 *db, const char *username, int *is_new) {
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, "SELECT id FROM users WHERE username = ?;", -1,
                          &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare select: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    if (is_new) *is_new = 0;
    return id;
  }
  sqlite3_finalize(stmt);

  if (sqlite3_prepare_v2(db, "INSERT INTO users (username) VALUES (?);", -1,
                          &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare insert: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Failed to insert user: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  if (is_new) *is_new = 1;
  return (int)sqlite3_last_insert_rowid(db);
}

int db_add_shoe(sqlite3 *db, int user_id, const char *brand,
                 const char *model, double size) {
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db,
                          "INSERT INTO shoes (user_id, brand, model, size) "
                          "VALUES (?, ?, ?, ?);",
                          -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare insert: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  sqlite3_bind_int(stmt, 1, user_id);
  sqlite3_bind_text(stmt, 2, brand, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, model, -1, SQLITE_STATIC);
  sqlite3_bind_double(stmt, 4, size);

  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Failed to insert shoe: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  return (int)sqlite3_last_insert_rowid(db);
}

int db_list_shoes(sqlite3 *db, int user_id) {
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db,
                          "SELECT brand, model, size FROM shoes "
                          "WHERE user_id = ? ORDER BY id;",
                          -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare select: %s\n", sqlite3_errmsg(db));
    return -1;
  }
  sqlite3_bind_int(stmt, 1, user_id);

  int count = 0;
  int rc;
  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    const unsigned char *brand = sqlite3_column_text(stmt, 0);
    const unsigned char *model = sqlite3_column_text(stmt, 1);
    double size = sqlite3_column_double(stmt, 2);
    printf("%d.) %s %s, size %.1f\n", count + 1, brand, model, size);
    count++;
  }
  sqlite3_finalize(stmt);

  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Failed to read shoes: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  return count;
}
