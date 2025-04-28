#pragma once
#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include <sqlite3.h>
#include <string>
#include <vector>

extern std::vector<std::string> browsers;
void showMenu(Vector2 &mousePosition, Font &font);

#endif
