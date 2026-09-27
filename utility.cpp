#include <string>

//Clamp function that takes min and max to clamp to
inline float clamp(float min, float value, float max) {
	//Can't go below min or above max
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

//Button state struct in the button
struct Button_State {
	bool is_down;
	bool changed;
};

//List of all the buttons, the numbers are first and in order cause
enum {
	BUTTON_0,
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
	BUTTON_6,
	BUTTON_7,
	BUTTON_8,
	BUTTON_9,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_W,
	BUTTON_S,
	BUTTON_A,
	BUTTON_D,
	BUTTON_P,
	BUTTON_I,

	//Last button in the array
	BUTTON_COUNT
};

//Input struct that stores all input info
struct Input {
	Button_State button[BUTTON_COUNT];
	int mouse_x;
	int mouse_y;
	bool is_mouse_down = false;
	bool mouse_was_just_pressed = false;
	bool mouse_is_just_pressed = false;
	bool mouse_just_pressed = false;
};

//Special Vector2, cause there are many things that have X and Y
struct Vector2 {
	float X;
	float Y;

	//Default 0
	Vector2() {
		X = 0;
		Y = 0;
	}

	Vector2(float x, float y) {
		X = x;
		Y = y;
	}
};

//Levels struct with the levels
struct Levels {
	int width, height;
	int size;
	const char* file_name;
	void* memory;
	int tile_size;
	Vector2 level_change;
	Levels() {
		level_change = { 10, 0 };
		tile_size = 40;
	}
};

struct Tile {
	int x, y;
	unsigned int colour;
};

enum colour {
	RED = 0xff0000,
	GREEN = 0x00ff00,
	BLUE = 0x0000ff,
	WHITE = 0xffffff,
	BLACK = 0x000000,
	ORANGE = 0xff5500,
	YELLOW = 0xffff00,
	PURPLE = 0x8000ff,
	CYAN = 0x00ffff,
	PINK = 0xFFB6C1,
	SKY_BLUE = 0x87CEEB
};

enum game_state_possible {
	EDITOR,
	PLAYING
};

class Player {
private:
	Vector2 position;
	Vector2 velocity;
	Vector2 direction;
	float gravity;
	float speed;
	int size;
	int half_size;
	float friction;
	bool is_on_ground;
	int facing_direction;

public:
	Player() {
		position = { 100, 100 };
		velocity = { 0, 0 };
		direction = { 0, 0 };
		size = 40;
		half_size = size / 2;
		gravity = 1500.f;
		speed = 300.f;
		friction = 0.f;
		is_on_ground = false;
		facing_direction = 1;
	}
	void render(const Vector2& camera_position);
	void move_and_collide(Input* input, float dt, const Levels& level);
	void collide_axis(const Levels& level, bool resolve_x);
	void is_on_ground_probe(const Levels& level);
	Vector2 player_position();
	int player_direction();
	void reset();
};

static void see_dimensions(float width, float height) {
	std::string message =
		"Width: " + std::to_string(width) +
		", Height: " + std::to_string(height) + "\n";

	OutputDebugStringA(message.c_str());
}

static int coords_to_tiles(int c, int tile_size) {
	return (c / tile_size);
}

static int tiles_to_coords(int c, int tile_size) {
	return (c * tile_size);
}

static unsigned int tile_id_to_colour(unsigned int colour) {
	if (colour) {
		switch (colour) {
			case 1: {
				return BLACK;
			}
			case 2: {
				return WHITE;
			}
			case 3: {
				return RED;
			}
			case 4: {
				return GREEN;
			}
			case 5: {
				return BLUE;
			}
			case 6: {
				return ORANGE;
			}
			case 7: {
				return YELLOW;
			}
			case 8: {
				return PURPLE;
			}
			case 9: {
				return CYAN;
			}
			default:
				return RED;
			}
	}
	else
		return 0;
}

static unsigned int colour_to_tile_id(unsigned int colour) {
	if (colour) {
		switch (colour) {
		case BLACK: {
			return 1;
		}
		case WHITE: {
			return 2;
		}
		case RED: {
			return 3;
		}
		case GREEN: {
			return 4;
		}
		case BLUE: {
			return 5;
		}
		case ORANGE: {
			return 6;
		}
		default:
			return RED;
		}
	}
	else
		return 0;
}