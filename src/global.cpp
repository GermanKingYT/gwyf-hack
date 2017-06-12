#include "global.h"

Ball *local_ball = NULL;
HUDInfo *hud_info = NULL;

bool *buddha_mode_opcode = NULL;
int current_map = 0;

std::vector<Vector*> global_positions;

HWND game_window = NULL;