#pragma once

#include <Windows.h>
#include <vector>

#include "gwyf.h"

extern Ball *local_ball;
extern HUDInfo *hud_info;

extern int current_map;

extern std::vector<Vector*> global_positions;

extern HWND game_window;