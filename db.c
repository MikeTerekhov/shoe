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
      ");";

  char *err_msg = NULL;
  if (sqlite3_exec(db, sql, NULL, NULL, &err_msg) != SQLITE_OK) {
    fprintf(stderr, "Failed to create users table: %s\n", err_msg);
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
