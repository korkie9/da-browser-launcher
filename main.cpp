#include "./utils/db.h"
#include "models/browser.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <raylib.h>
#include <sqlite3.h>
#include <string>
#include <vector>
#define MAX_INPUT_CHARS 20

using namespace std;

struct Browser;
int getProfilesCallback(void *data, int argc, char **argv, char **azColName);
int getBrowsersCallback(void *data, int argc, char **argv, char **azColName);
void openBrowserAndCloseProgram(sqlite3 *db, Texture2D *convertableTexture,
                                string cmd, Font *font, Image *listIcon,
                                Image *addIcon);

void showMenu(Vector2 &mousePosition, Font &font, sqlite3 *db,
              vector<Browser> &browserDbList, string *screen);
struct Profile {
  string id;
  string name;
};

int main() {
  sqlite3 *db;
  openDb(&db);
  Profile profile;
  vector<Profile> profiles;
  string sqlQuery = "SELECT * FROM Profiles;";
  sqlite3_exec(db, sqlQuery.c_str(), getProfilesCallback, &profiles, nullptr);
  const int screenWidth = 800;
  const int screenHeight = 600;
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
  InitWindow(screenWidth, screenHeight, "Raylib Button Example");
  SetWindowOpacity(0.5f);
  const char *selectedFilePath = nullptr;
  bool showAddProfileTextBox = false;
  Rectangle profileTextBox = {screenWidth / 2.0f - 150, 180, 425, 50};
  string text;
  SetExitKey(KEY_NULL);
  Texture2D background = LoadTexture("assets/dababy.png");
  Image convertable = LoadImage("assets/dababy-convertable.png");
  Texture2D convertableTexture = LoadTextureFromImage(convertable);
  UnloadImage(convertable);
  background.width = screenWidth;
  float scrollingBack = 0.0f;
  int index = 0;
  bool editProfile = false;
  vector<Browser> browsers;
  Font fontTtf = LoadFontEx("assets/EagleLake-Regular.ttf", 300, 0, 250);
  string getBrowserQuery = "SELECT * FROM Browsers;";
  sqlite3_exec(db, getBrowserQuery.c_str(), getBrowsersCallback, &browsers,
               nullptr);
  string screen = "home";
  Image addIcon = LoadImage("assets/add.png");
  Texture2D addIconTexture = LoadTextureFromImage(addIcon);
  Image listIcon = LoadImage("assets/list.png");
  Texture2D listIconTexture = LoadTextureFromImage(listIcon);

  while (!WindowShouldClose()) {

    scrollingBack -= 0.1f;
    if (showAddProfileTextBox) {
      int key = GetCharPressed();
      if ((key >= 32) && (key <= 125) && text.size() < 22) {
        text += (char)key;
      }
    }
    if (scrollingBack <= -background.width * 2)
      scrollingBack = 0;
    if ((IsKeyPressed(KEY_J) || IsKeyPressed(KEY_DOWN)) &&
        !showAddProfileTextBox) {
      if (index + 1 < static_cast<int>(profiles.size())) {
        index++;
      }
    }
    if (IsKeyPressed(KEY_R) && !showAddProfileTextBox) {
      showAddProfileTextBox = true;
      editProfile = true;
      text = "";
    }
    if ((IsKeyPressed(KEY_K) || IsKeyPressed(KEY_UP)) &&
        !showAddProfileTextBox) {
      if (index - 1 > -1) {
        index--;
      }
    }
    if (IsKeyPressed(KEY_D) && !showAddProfileTextBox) {
      if (index >= 0 && index <= (int)profiles.size() && profiles.size() > 0) {

        string deleteUserQuery =
            "DELETE FROM Profiles WHERE name = '" + profiles[index].name + "';";
        char *errMsg = nullptr;

        int rc = sqlite3_exec(db, deleteUserQuery.c_str(), getProfilesCallback,
                              &profiles, &errMsg);

        if (rc != SQLITE_OK) {
          std::cerr << "SQL error: " << errMsg << std::endl;
        } else {
          profiles.erase(profiles.begin() + index);
          showAddProfileTextBox = false;
        }
        sqlite3_free(errMsg);
      }
    }
    if (IsKeyPressed(KEY_ENTER)) {
      if (!showAddProfileTextBox && !browsers.empty()) {
        string cmd = "";
        if (browsers[0].path == "Firefox") {
          cmd = cmd + "firefox" + " -P \"" + profiles[index].name + "\" &";
        }
        if (browsers[0].path == "Brave")
          cmd = cmd + "brave" + " --profile-directory=\"" +
                profiles[index].name + "\" &";
        if (browsers[0].path == "Chrome")
          cmd = cmd + "google-chrome-stable" + " --profile-directory=\"" +
                profiles[index].name + "\" &";
        if (browsers[0].path == "Zen")
          cmd = "flatpak run app.zen_browser.zen -p \"" + profiles[index].name +
                "\" &";

        cout << (cmd) << endl;
        openBrowserAndCloseProgram(db, &convertableTexture, cmd, &fontTtf,
                                   &listIcon, &addIcon);
      }
      if (showAddProfileTextBox) {

        if (!editProfile) {

          if (text.size() > 0) {
            string addUserQuery =
                "INSERT INTO Profiles (name) VALUES ('" + text + "');";
            char *errMsg = nullptr;

            int rc = sqlite3_exec(db, addUserQuery.c_str(), getProfilesCallback,
                                  &profiles, nullptr);
            if (rc != SQLITE_OK) {
              std::cerr << "SQL error: " << errMsg << std::endl;
              sqlite3_free(errMsg);
            } else {
              profiles.push_back({to_string(profiles.size() + 1), text});
              showAddProfileTextBox = false;
            }
          }
        } else {

          if (text.size() > 0) {
            string updateUserQuery = "UPDATE Profiles SET name = '" + text +
                                     "' WHERE name = '" + profiles[index].name +
                                     "';";
            char *errMsg = nullptr;

            int rc = sqlite3_exec(db, updateUserQuery.c_str(), nullptr,
                                  &profiles, nullptr);
            if (rc != SQLITE_OK) {
              std::cerr << "SQL error: " << errMsg << std::endl;
              sqlite3_free(errMsg);
            } else {
              profiles[index].name = text;
              showAddProfileTextBox = false;
              editProfile = false;
              sqlite3_free(errMsg);
            }
          }
        }
      }

      text = "";
    }
    if (showAddProfileTextBox) {
      if (IsKeyPressed(KEY_BACKSPACE)) {
        if (text.size() > 0)
          text.pop_back();
      }
      if (IsKeyPressed(KEY_ESCAPE)) {
        showAddProfileTextBox = false;
      }
    }

    Vector2 mousePosition = GetMousePosition();

    Color selectedbuttonhovercolor = DARKGRAY;
    Color addProfileBtnColor = DARKGRAY;
    Rectangle selectBrowserBtn = {(float)(screenWidth + 600) / 2, 25, 60, 60};

    Rectangle addProfileBtn = {(float)(screenWidth + 600) / 2,
                               screenHeight - 85, 60, 60};

    if (CheckCollisionPointRec(mousePosition, addProfileBtn)) {
      addProfileBtnColor = LIGHTGRAY;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        showAddProfileTextBox = true;
      }

    } else {
      addProfileBtnColor = DARKGRAY;
    }

    if (CheckCollisionPointRec(mousePosition, selectBrowserBtn)) {
      selectedbuttonhovercolor = LIGHTGRAY;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        screen = "menu";
      }

    } else {
      selectedbuttonhovercolor = DARKGRAY;
    }

    // Draw
    BeginDrawing();
    ClearBackground(BLANK);

    const float buttonHeight = 60;
    const float buttonSpacing = 10;

    if (screen == "home") {

      for (size_t i = 0; i < profiles.size(); i++) {
        Color buttonColor = Color{66, 135, 245, 70};
        Color buttonHoverColor = Color{66, 135, 245, 200};
        Rectangle button = {(float)50,
                            (float)(20 + (buttonHeight + buttonSpacing) * i),
                            600, buttonHeight};

        if (CheckCollisionPointRec(mousePosition, button)) {
          buttonColor = buttonHoverColor;
          if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !browsers.empty()) {
            string cmd = browsers[0].path + " -P " + profiles[i].name + " &";
            openBrowserAndCloseProgram(db, &convertableTexture, cmd, &fontTtf,
                                       &listIcon, &addIcon);
          }
        } else {
          buttonColor = Color{66, 135, 245, 70};
        }
        if (index == (int)i) {
          buttonColor = Color{66, 135, 245, 200};
          ;
        }
        DrawRectangleRounded(button, 0.3f, 10, buttonColor);
        DrawTextEx(fontTtf, profiles[i].name.c_str(),
                   (Vector2){button.x + 30, button.y + 15}, 43.0f, 5.0f, BLACK);
      }
    } else {
      showMenu(mousePosition, fontTtf, db, browsers, &screen);
    }

    if (showAddProfileTextBox) {

      DrawRectangleRounded(profileTextBox, 0.3f, 10, LIGHTGRAY);
      DrawRectangleRoundedLines(profileTextBox, 0.3f, 10, BLUE);
      DrawTextEx(fontTtf, text.c_str(),
                 (Vector2){profileTextBox.x + 5, profileTextBox.y + 8}, 40.0f,
                 5.0f, MAROON);
    }

    // Add profile button
    Rectangle addRect = {0.0f, 0.0f, (float)addIconTexture.width,
                         (float)addIconTexture.height};

    DrawTexturePro(addIconTexture, addRect, addProfileBtn, Vector2{0, 0}, 0.0f,
                   WHITE);

    // Select browser button
    Rectangle sourceRect = {0.0f, 0.0f, (float)listIconTexture.width,
                            (float)listIconTexture.height};
    //
    DrawTexturePro(listIconTexture, sourceRect, selectBrowserBtn, Vector2{0, 0},
                   0.0f, WHITE);

    if (profiles.empty()) {
      DrawText("No profile has been set", 10, 15, 24, RED);
      DrawText("(Click DaPlus (+) sign to add profile)", 10, 35, 18, RED);
    }
    if (browsers.empty()) {
      DrawText("DaBrowser has not been set", 10, screenHeight - 75, 22, RED);
      DrawText("(Click DaConvertible to add set brower) =>", 10,
               screenHeight - 45, 16, RED);
    }
    EndDrawing();
  }

  UnloadTexture(convertableTexture);

  // Close the window
  if (db) {
    sqlite3_close(db);
  }
  CloseWindow();
  return 0;
}

int getProfilesCallback(void *data, int argc, char **argv, char **azColName) {
  vector<Profile> *result = static_cast<vector<Profile> *>(data);
  if (argc > 0 && argv[0] != nullptr) {
    result->push_back({argv[0], argv[1]});
  }
  return 0;
}

int getBrowsersCallback(void *data, int argc, char **argv, char **azColName) {
  vector<Browser> *result = static_cast<vector<Browser> *>(data);
  if (argc > 0 && argv[0] != nullptr) {
    result->push_back({argv[0], argv[1]});
  }
  return 0;
}

void openBrowserAndCloseProgram(sqlite3 *db, Texture2D *convertableTexture,
                                string cmd, Font *font, Image *listIcon,
                                Image *addIcon) {
  system(cmd.c_str());
  if (db)
    sqlite3_close(db);
  UnloadTexture(*convertableTexture);
  UnloadFont(*font);
  UnloadImage(*listIcon);
  UnloadImage(*addIcon);
  CloseWindow();
  exit(0);
}
