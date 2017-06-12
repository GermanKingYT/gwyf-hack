#include "features.h"

#include <Windows.h>
#include <iostream>

#include "global.h"


void modify_stroke_counter()
{
	if (GetAsyncKeyState(VK_ADD) & 1)
		local_ball->stroke_counter++;
	else if (GetAsyncKeyState(VK_SUBTRACT) & 1)
		local_ball->stroke_counter--;
}

void do_airstuck()
{
	static bool once = false;
	static float speed_modifier_backup = 0.5f;

	if (GetAsyncKeyState(VK_MBUTTON) && !local_ball->is_out_of_map)
	{
		if (!once)
		{
			speed_modifier_backup = local_ball->speed_modifier;
			once = true;
		}

		local_ball->speed_modifier = 200.f;
	}
	else
	{
		if (once)
		{
			local_ball->set_buddha_mode(true);

			local_ball->speed_modifier = speed_modifier_backup;
			once = false;
		}
		else if (local_ball->speed > local_ball->min_speed && local_ball->buddha_mode) //are we moving and did we successfully apply the buddha mode to the local ball struct?
		{
			local_ball->set_buddha_mode(false);

			local_ball->stroke_counter++;
		}
	}
}

void do_walk_mode()
{
	static bool once = false;

	if (GetAsyncKeyState('W'))
	{
		local_ball->min_speed = 9999.f;
		local_ball->set_buddha_mode(true);

		local_ball->stroke_power_sens = 60.f;
		local_ball->current_stroke_power = 10.f;
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(1);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

		once = false;
	}
	else
	{
		if (!once)
		{
			local_ball->min_speed = 0.1f;
			local_ball->stroke_power_sens = 360.f;
			local_ball->set_buddha_mode(false);

			once = true;
		}
	}
}

void do_teleport()
{
	if (GetAsyncKeyState(VK_F2))
	{
		for (int i = 0; i < global_positions.size(); i++)
		{
			const float holes[6][3] = { { 40.851376f, -0.726303f, 23.997446f },
										{ -85.830963f, 14.873729f, -206.772873f },
										{ -37.975327f, 2.459825f, 61.104744f },
										{ 45.001198f, 0.979223f, 53.913559f },
										{ 131.122009f, 19.377558f, -40.999252f },
										{ 344.970795f, 4.640485f, 155.016922f } };

			if (current_map < 0 || current_map > 6)
				return;

			memcpy(global_positions[i], holes[current_map - 1], sizeof(float) * 3);
		}
	}
}

void main_thread()
{
	while (true)
	{
		if (local_ball != NULL)
		{
			if (GetForegroundWindow() == game_window)
			{
				modify_stroke_counter();
				do_airstuck();
				do_walk_mode();

				do_teleport();
			}
		}

		Sleep(20);
	}
}