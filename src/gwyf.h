#pragma once
#include <stdint.h>

extern bool *buddha_mode_opcode;

#pragma pack(push, 1)
class Ball
{
public:

	uint8_t __padheheh[0xFC];
	float fov;
	uint8_t __pad[0x20];
	float speed;
	float movement_dir[3];
	uint8_t __pad2[0x20];
	int max_strokes;
	int stroke_counter;
	uint8_t __padlolo[0xC];
	bool is_out_of_map;
	uint8_t __pad3[0x17];
	float current_stroke_power;
	float max_stroke_power;
	float min_stroke_power;
	float stroke_power_sens;
	float stroke_power_sens2;
	float min_speed; //0.1 always, compared with speed probably to detect movement
	uint8_t __pad5[0x14];
	float speed_modifier;
	uint8_t __pad6[0x34];
	float ball_position[3];
	uint8_t __pad7[0x62];
	bool buddha_mode; //if this is set to true we can shoot from air and stroke counter doesn't increase
	uint8_t __pad8[6];
	float frane_counter;

	void set_buddha_mode(bool set)
	{
		buddha_mode = set;
		*buddha_mode_opcode = set;
	}

};

class HUDInfo
{
public:
	char _0x0000[272];
	__int32 max_round_time; //0x0100 max game time in sekunden
	char _0x0104[84];
	float current_power; //0x0158 power von 0-100%
	char _0x015C[4];
	__int32 total_score; //0x0160 current scoreboard score
	char _0x0164[8];
	__int32 dritte; //0x0170 sekunde zweite stelle nach komma
	__int32 zweite; //0x0174 sekunde erste stelle nach komma
	__int32 erste; //0x0178  minute
	__int32 round_time; //0x017C game time in sekunden
	__int32 is_playing; //0x0180 ingame
	char _0x0184[32];
	float gravity; //0x019C -30 standard / positive value == abheben
	char _0x01A0[4];
	float acceleration; //0x01A4 read only acc
	char _0x01A8[4];
	__int32 stroke_counter; //0x01AC stroke counter wie im ball struct
	char _0x01B0[16];
	__int32 spec_time; //0x01C0	none / 15secs / infinite
	__int32 max_spec_time; //0x01C4 none / 15secs / infinite
	char _0x01C8[4];
	__int32 rounds_played; //0x01CC 
	char _0x01D0[120];
};//Size=0x0248
#pragma pack(pop, 1)

struct Vector
{
	float x, z, y;
};

/*enum MapIDs
{
	kUndefined,
	kForest,
	kOasis,
	kTwilight,
	kHaunted,
	kCandyland,
	kAncient
};*/