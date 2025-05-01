#include "./utils/db/db.h"
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
void openBrowserAndCloseProgram(sqlite3 *db, string cmd, Font *font,
                                Texture2D *addIconTexture,
                                Texture2D *listIconTexture,
                                Texture2D *houseIconTexture, Image *addIcon,
                                Image *listIcon, Image *houseIcon);

void cleanupResources(sqlite3 *db, Texture2D *addIconTexture,
                      Texture2D *listIconTexture, Texture2D *houseIconTexture,
                      Font *font, Image *addIcon, Image *listIcon,
                      Image *houseIcon);

string find_resource(const string &fileName);
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
  bool showAddProfileTextBox = false;
  Rectangle profileTextBox = {screenWidth / 2.0f - 160, 180, 450, 60};
  string text;
  SetExitKey(KEY_NULL);
  float scrollingBack = 0.0f;
  int index = 0;
  bool editProfile = false;
  vector<Browser> browsers;
  string fontStr = "EagleLake-Regular.ttf";
  Font fontTtf = LoadFontEx(find_resource(fontStr).c_str(), 300, 0, 250);
  string getBrowserQuery = "SELECT * FROM Browsers;";
  sqlite3_exec(db, getBrowserQuery.c_str(), getBrowsersCallback, &browsers,
               nullptr);
  string screen = "home";
  string addImgStr = "add.png";
  string listImgStr = "list.png";
  string houseImgStr = "house.png";
  Image addIcon = LoadImage(find_resource(addImgStr).c_str());
  Texture2D addIconTexture = LoadTextureFromImage(addIcon);
  Image listIcon = LoadImage(find_resource(listImgStr).c_str());
  Texture2D listIconTexture = LoadTextureFromImage(listIcon);
  Image houseIcon = LoadImage(find_resource(houseImgStr).c_str());
  Texture2D houseIconTexture = LoadTextureFromImage(houseIcon);

  while (!WindowShouldClose()) {

    scrollingBack -= 0.1f;
    if (showAddProfileTextBox) {
      int key = GetCharPressed();
      if ((key >= 32) && (key <= 125) && text.size() < 22) {
        text += (char)key;
      }
    }
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
        openBrowserAndCloseProgram(db, cmd, &fontTtf, &addIconTexture,
                                   &listIconTexture, &houseIconTexture,
                                   &addIcon, &listIcon, &houseIcon);
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

    if (IsKeyPressed(KEY_ESCAPE)) {
      if (screen != "home" && !showAddProfileTextBox) {
        screen = "home";
      }
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

    if (IsKeyPressed(KEY_A)) {
      showAddProfileTextBox = true;
    }

    if (CheckCollisionPointRec(mousePosition, selectBrowserBtn)) {
      selectedbuttonhovercolor = LIGHTGRAY;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (screen == "home") {
          screen = "browserList";
        } else if (screen == "browserList") {
          screen = "home";
        }
      }

    } else {
      selectedbuttonhovercolor = DARKGRAY;
    }

    if (IsKeyPressed(KEY_S) && !showAddProfileTextBox) {

      if (screen == "home") {
        screen = "browserList";
      } else if (screen == "browserList") {
        screen = "home";
      }
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
            if (!showAddProfileTextBox && !browsers.empty()) {
              string cmd = "";
              if (browsers[0].path == "Firefox") {
                cmd =
                    cmd + "firefox" + " -P \"" + profiles[index].name + "\" &";
              }
              if (browsers[0].path == "Brave")
                cmd = cmd + "brave" + " --profile-directory=\"" +
                      profiles[index].name + "\" &";
              if (browsers[0].path == "Chrome")
                cmd = cmd + "google-chrome-stable" + " --profile-directory=\"" +
                      profiles[index].name + "\" &";
              if (browsers[0].path == "Zen")
                cmd = "flatpak run app.zen_browser.zen -p \"" +
                      profiles[index].name + "\" &";

              cout << (cmd) << endl;
              openBrowserAndCloseProgram(db, cmd, &fontTtf, &addIconTexture,
                                         &listIconTexture, &houseIconTexture,
                                         &addIcon, &listIcon, &houseIcon);
            }
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

      DrawRectangleRounded(profileTextBox, 0.3f, 10, Color{71, 144, 166, 200});
      DrawRectangleRoundedLines(profileTextBox, 0.3f, 10, BLUE);
      DrawTextEx(fontTtf, text.c_str(),
                 (Vector2){profileTextBox.x + 12, profileTextBox.y + 8}, 40.0f,
                 5.0f, BLACK);
    }

    // Add profile button
    Rectangle addRect = {0.0f, 0.0f, (float)addIconTexture.width,
                         (float)addIconTexture.height};

    DrawTexturePro(addIconTexture, addRect, addProfileBtn, Vector2{0, 0}, 0.0f,
                   WHITE);

    // Select browser button
    Rectangle houseSourceRect = {0.0f, 0.0f, (float)houseIconTexture.width,
                                 (float)houseIconTexture.height};

    Rectangle listSourceRect = {0.0f, 0.0f, (float)listIconTexture.width,
                                (float)listIconTexture.height};
    // heere
    if (screen == "browserList") {
      DrawTexturePro(houseIconTexture, houseSourceRect, selectBrowserBtn,
                     Vector2{0, 0}, 0.0f, WHITE);
    } else {
      DrawTexturePro(listIconTexture, listSourceRect, selectBrowserBtn,
                     Vector2{0, 0}, 0.0f, WHITE);
    }

    if (profiles.empty() && screen == "home") {
      DrawTextEx(fontTtf, "No profile has been set", (Vector2){10, 15}, 36.0f,
                 3.0f, RED);
      DrawTextEx(fontTtf, "Click the plus (+) sign to add profile",
                 (Vector2){10, 50}, 36.0f, 3.0f, RED);
    }
    EndDrawing();
  }

  // manual cleanup
  cleanupResources(db, &addIconTexture, &listIconTexture, &houseIconTexture,
                   &fontTtf, &addIcon, &listIcon, &houseIcon);
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

void openBrowserAndCloseProgram(sqlite3 *db, string cmd, Font *font,
                                Texture2D *addIconTexture,
                                Texture2D *listIconTexture,
                                Texture2D *houseIconTexture, Image *addIcon,
                                Image *listIcon, Image *houseIcon) {
  system(cmd.c_str());
  cleanupResources(db, addIconTexture, listIconTexture, houseIconTexture, font,
                   addIcon, listIcon, houseIcon);
  CloseWindow();
  exit(0);
}

void cleanupResources(sqlite3 *db, Texture2D *addIconTexture,
                      Texture2D *listIconTexture, Texture2D *houseIconTexture,
                      Font *font, Image *addIcon, Image *listIcon,
                      Image *houseIcon) {
  if (db)
    sqlite3_close(db);
  if (addIconTexture)
    UnloadTexture(*addIconTexture);
  if (listIconTexture)
    UnloadTexture(*listIconTexture);
  if (houseIconTexture)
    UnloadTexture(*houseIconTexture);
  if (font)
    UnloadFont(*font);
  if (addIcon)
    UnloadImage(*addIcon);
  if (listIcon)
    UnloadImage(*listIcon);
  if (houseIcon)
    UnloadImage(*houseIcon);
}
