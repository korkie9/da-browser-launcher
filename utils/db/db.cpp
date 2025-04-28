#include <filesystem>
#include <iostream>
#include <raylib.h>
#include <sqlite3.h>

using namespace std;

bool openDb(sqlite3 **db) {
  char *errMsg = nullptr;

  // Open database (creates it if it doesn't exist)
  const char *configDir = getenv("XDG_CONFIG_HOME");
  string dbDir = configDir ? configDir : string(getenv("HOME")) + "/.config";
  dbDir += "/da-browser-launcher";
  filesystem::create_directories(dbDir);
  string dbPath = dbDir + "/database.db";

  if (sqlite3_open(dbPath.c_str(), db) != SQLITE_OK) {
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
    return false;
  } else {
    cout << "Table created successfully." << endl;
  }
  return true;
}
