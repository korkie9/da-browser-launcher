#include <raylib.h>
#include <sqlite3.h>
#include <string>
#include <vector>

using namespace std;

vector<string> browsers = {"Firefox", "Brave", "Chrome", "Zen"};

void showMenu(Vector2 &mousePosition, Font &font) {

  Color buttonColor = Color{66, 135, 245, 70};

  const float buttonHeight = 60;
  const float buttonSpacing = 10;
  for (int i = 0; i < browsers.size(); i++) {
    Color buttonColor = Color{66, 135, 245, 70};

    Color buttonHoverColor = Color{66, 135, 245, 200};
    Rectangle button = {(float)50,
                        (float)(20 + (buttonHeight + buttonSpacing) * i), 600,
                        buttonHeight};

    // here
    if (CheckCollisionPointRec(mousePosition, button)) {
      buttonColor = buttonHoverColor;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !browsers.empty()) {
        // openBrowserAndCloseProgram(db, &convertableTexture, cmd);
      }
    } else {
      buttonColor = Color{66, 135, 245, 70};
    }
    DrawRectangleRounded(button, 0.3f, 10, buttonColor);
    DrawTextEx(font, browsers[i].c_str(),
               (Vector2){button.x + 30, button.y + 15}, 43.0f, 5.0f, BLACK);
  }
}
