#include "../models/browser.h"
#include <iostream>
#include <raylib.h>
#include <sqlite3.h>
#include <string>
#include <vector>

using namespace std;

vector<string> browsers = {"Firefox", "Brave", "Chrome", "Zen"};

void addBrowser(string browser, sqlite3 *db, vector<Browser> &browserDbList,
                string *screen);

void showMenu(Vector2 &mousePosition, Font &font, sqlite3 *db,
              vector<Browser> &browserDbList, string *screen) {

  Color buttonInactiveColor = Color{66, 135, 245, 70};
  Color buttonHoverColor = Color{66, 135, 245, 200};
  Color buttonColor = Color{66, 135, 245, 200};

  const float buttonHeight = 60;
  const float buttonSpacing = 10;
  for (int i = 0; i < (int)browsers.size(); i++) {
    Rectangle button = {(float)50,
                        (float)(20 + (buttonHeight + buttonSpacing) * i), 600,
                        buttonHeight};

    if (CheckCollisionPointRec(mousePosition, button)) {
      buttonColor = buttonHoverColor;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !browsers.empty()) {
        addBrowser(browsers[i], db, browserDbList, screen);
      }
    } else {
      buttonColor = buttonInactiveColor;
    }
    DrawRectangleRounded(button, 0.3f, 10, buttonColor);
    DrawTextEx(font, browsers[i].c_str(),
               (Vector2){button.x + 30, button.y + 15}, 43.0f, 5.0f, BLACK);
  }
}

int browserCallback(void *data, int argc, char **argv, char **azColName) {
  vector<string> *browsers = static_cast<vector<string> *>(data);
  if (argc > 0 && argv[0]) {
    browsers->push_back(argv[0]);
  }
  return 0;
}

void addBrowser(string browser, sqlite3 *db, vector<Browser> &browserDbList,
                string *screen) {

  char *errMsg = nullptr;
  string addBrowserQuery =
      "UPDATE Browsers SET path ='" + browser + "' WHERE id=1;";

  int rc = sqlite3_exec(db, addBrowserQuery.c_str(), nullptr, &browserDbList,
                        &errMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  } else {
    browserDbList[0].path = browser;
    *screen = "home";
  }
}
