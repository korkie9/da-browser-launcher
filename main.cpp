#include "./libtinyfiledialogs/tinyfiledialogs.h"
#include "./utils/db.h"
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
int callback(void *data, int argc, char **argv, char **azColName);
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
  sqlite3_exec(db, sqlQuery.c_str(), callback, &profiles, nullptr);
  const int screenWidth = 800;
  const int screenHeight = 600;
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
  InitWindow(screenWidth, screenHeight, "Raylib Button Example");
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

  while (!WindowShouldClose()) {

    scrollingBack -= 0.1f;

    if (scrollingBack <= -background.width * 2)
      scrollingBack = 0;
    if (IsKeyPressed(KEY_J) && !showAddProfileTextBox) {
      cout << static_cast<int>(profiles.size()) << endl;
      if (index + 1 < static_cast<int>(profiles.size())) {
        index++;
        cout << "Incrementing: " << index << endl;
      }
    }
    if (IsKeyPressed(KEY_K) && !showAddProfileTextBox) {
      if (index - 1 > -1) {

        index--;
        cout << "Decrementing" << index << endl;
      }
    }
    if (IsKeyPressed(KEY_D) && !showAddProfileTextBox) {
      if (index >= 0 && index <= (int)profiles.size() && profiles.size() > 0) {

        cout << "Deleting " << index << endl;
        string deleteUserQuery =
            "DELETE FROM Profiles WHERE name = '" + profiles[index].name + "'";
        char *errMsg = nullptr;

        int rc = sqlite3_exec(db, deleteUserQuery.c_str(), callback, &profiles,
                              nullptr);
        if (rc != SQLITE_OK) {
          std::cerr << "SQL error: " << errMsg << std::endl;
          sqlite3_free(errMsg);
        } else {
          profiles.erase(profiles.begin() + index);
          showAddProfileTextBox = false;
        }
      }
    }
    if (IsKeyPressed(KEY_ENTER)) {
      if (!showAddProfileTextBox) {
        string tempBrowser =
            "/home/justin/Documents/browsers/zen/zen-generic.AppImage";
        string cmd = tempBrowser + " -P " + profiles[index].name;
        cout << cmd << endl;
        int hasLaunched = system(cmd.c_str());
        if (hasLaunched == 0)
          return 0;
        cout << hasLaunched << endl;
      }
      if (showAddProfileTextBox) {

        if (text.size() > 0) {
          string addUserQuery =
              "INSERT INTO Profiles (name) VALUES ('" + text + "')";
          char *errMsg = nullptr;

          int rc = sqlite3_exec(db, addUserQuery.c_str(), callback, &profiles,
                                nullptr);
          if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
          } else {
            profiles.push_back({to_string(profiles.size() + 1), text});
            showAddProfileTextBox = false;
          }
        }
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
    Rectangle selectedFilePathBtn = {(float)(screenWidth - 60) / 2,
                                     screenHeight - 85, 60, 60};

    Rectangle addProfileBtn = {(float)(screenWidth + 400) / 2,
                               screenHeight - 85, 60, 60};

    if (CheckCollisionPointRec(mousePosition, addProfileBtn)) {
      addProfileBtnColor = LIGHTGRAY;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        showAddProfileTextBox = true;
        cout << selectedFilePath << endl;
      }

    } else {
      addProfileBtnColor = DARKGRAY;
    }

    if (CheckCollisionPointRec(mousePosition, selectedFilePathBtn)) {
      selectedbuttonhovercolor = LIGHTGRAY;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        selectedFilePath =
            tinyfd_openFileDialog("Choose browser file", "", 0, NULL, NULL, 0);
        cout << selectedFilePath << endl;
      }

    } else {
      selectedbuttonhovercolor = DARKGRAY;
    }

    // Draw
    BeginDrawing();
    ClearBackground(GetColor(0x052c46ff));

    DrawTexturePro(
        background,
        Rectangle{0, 0, (float)background.width, (float)background.height},
        Rectangle{scrollingBack, 0, (float)screenWidth, (float)screenHeight},
        Vector2{0, 0}, 0.0f, WHITE);

    DrawTexturePro(
        background,
        Rectangle{0, 0, (float)background.width, (float)background.height},
        Rectangle{scrollingBack + screenWidth, 0, (float)screenWidth,
                  (float)screenHeight},
        Vector2{0, 0}, 0.0f, WHITE);
    DrawTexturePro(
        background,
        Rectangle{0, 0, (float)background.width, (float)background.height},
        Rectangle{scrollingBack + (screenWidth * 2), 0, (float)screenWidth,
                  (float)screenHeight},
        Vector2{0, 0}, 0.0f, WHITE);
    const float buttonHeight = 60;
    const float buttonSpacing = 10;

    for (size_t i = 0; i < profiles.size(); i++) {
      Color buttonColor = DARKGRAY;
      Color buttonHoverColor = LIGHTGRAY;
      Rectangle button = {(float)50,
                          (float)(20 + (buttonHeight + buttonSpacing) * i), 600,
                          buttonHeight};

      if (CheckCollisionPointRec(mousePosition, button)) {
        buttonColor = buttonHoverColor;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          cout << selectedFilePath << endl;
          cout << "Button clicked: " << profiles[i].name << endl;
          printf("%s, clieck; \n", profiles[i].name.c_str());
        }
      } else {
        buttonColor = DARKGRAY;
      }
      if (index == (int)i) {
        buttonColor = LIGHTGRAY;
      }
      DrawRectangleRounded(button, 0.3f, 10, buttonColor);
      DrawText(profiles[i].name.c_str(), button.x + 30, button.y + 15, 20,
               BLACK);
    }

    if (showAddProfileTextBox) {

      int key = GetCharPressed();
      if ((key >= 32) && (key <= 125) && text.size() < 22) {
        text += (char)key;
        cout << text << endl;
      }

      DrawRectangleRounded(profileTextBox, 0.3f, 10, LIGHTGRAY);
      DrawRectangleRoundedLines(profileTextBox, 0.3f, 10, 2.0f, BLUE);
      DrawText(text.c_str(), (int)profileTextBox.x + 5,
               (int)profileTextBox.y + 8, 40, MAROON);
    }

    DrawRectangleRounded(selectedFilePathBtn, 0.3f, 10,
                         selectedbuttonhovercolor);
    DrawRectangleRoundedLines(selectedFilePathBtn, 0.3f, 10, 2.0f, BLUE);

    DrawRectangleRounded(addProfileBtn, 0.3f, 10, addProfileBtnColor);
    DrawRectangleRoundedLines(addProfileBtn, 0.3f, 10, 2.0f, BLUE);
    DrawText("+", addProfileBtn.x + 20, addProfileBtn.y + (addProfileBtn.y / 2),
             50, BLACK);

    Rectangle sourceRect = {0.0f, 0.0f, (float)convertableTexture.width,
                            (float)convertableTexture.height};

    DrawTexturePro(
        convertableTexture,
        sourceRect,          // source rectangle
        selectedFilePathBtn, // destination rectangle (scaled to window)
        Vector2{0, 0}, 0.0f, WHITE);

    EndDrawing();
  }

  UnloadTexture(convertableTexture); // Texture unloading

  // Close the window
  if (db) {
    sqlite3_close(db);
  }
  CloseWindow();
  return 0;
}

int callback(void *data, int argc, char **argv, char **azColName) {
  vector<Profile> *result = static_cast<vector<Profile> *>(data);
  if (argc > 0 && argv[0] != nullptr) {
    result->push_back({argv[0], argv[1]});
  }
  return 0;
}
