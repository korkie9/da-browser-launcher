#include <iostream>
#include <raylib.h>
#include <sqlite3.h>

using namespace std;

// Function to open a database and create a table if it doesn't exist
bool openDb(sqlite3 **db) {
  char *errMsg = nullptr; // Pointer for error message

  // Open the database (creates it if it doesn't exist)
  if (sqlite3_open("database.db", db) != SQLITE_OK) {
    cerr << "Can't open database: " << sqlite3_errmsg(*db) << endl;
    CloseWindow();
    return false;
  }

  const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS Profiles ("
                               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "NAME TEXT NOT NULL);"
                               "CREATE TABLE IF NOT EXISTS Browsers ("
                               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "PATH TEXT NOT NULL);"
                               "INSERT INTO Browsers (path) VALUES ('Firefox')";

  if (sqlite3_exec(*db, sqlCreateTable, nullptr, 0, &errMsg) != SQLITE_OK) {
    cerr << "SQL error: " << errMsg << endl;
    sqlite3_free(errMsg);
    return false; // Return false if there was an error
  } else {
    cout << "Table created successfully." << endl;
  }
  return true;
}
