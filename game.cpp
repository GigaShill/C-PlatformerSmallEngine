#include <cstdlib>
#define is_down(b) input->button[b].is_down
#define pressed(b) (input->button[b].is_down && input->button[b].changed)
#define released(b) (!input->button[b].is_down && input->button[b].changed)

LARGE_INTEGER seed;
LARGE_INTEGER frame_time;

static game_state_possible game_state = PLAYING;
static int colour_tiles = 3;

static double init_frame_time, cur_frame_time;

Player player;

static void change_level(Levels* level_to_load, Levels* cur_level, Player* player) {
	level_free_data(cur_level);
	player->reset();
	camera_reset(player);
	setup_level(level_to_load, level_to_load->file_name, level_to_load->tile_size);
	current_level = level_to_load;
}

static void change_level_editor(Levels* level_to_load, Levels* cur_level) {
	level_free_data(cur_level);
	camera_reset_editor();
	setup_level(level_to_load, level_to_load->file_name, level_to_load->tile_size);
	current_level = level_to_load;
}

static void check_level_change(Levels* current_level, Levels* next_level, Player* player) {
	if (current_level->level_change.X == 0) {
		if (coords_to_tiles(player->player_position().Y, current_level->tile_size) > (int)current_level->level_change.Y) {
			change_level(next_level, current_level, player);
		}
	}
	else if (current_level->level_change.Y == 0) {
		if (coords_to_tiles(player->player_position().X, current_level->tile_size) > (int)current_level->level_change.X) {
			change_level(next_level, current_level, player);
		}
	}
	else {
		if (coords_to_tiles(player->player_position().X, current_level->tile_size) > current_level->level_change.X && 
			coords_to_tiles(player->player_position().Y, current_level->tile_size) > current_level->level_change.Y) {
			change_level(next_level, current_level, player);
		}
	}
}

static void change_level_editor_call(Levels* current_level, Levels* next_level) {
	change_level_editor(next_level, current_level);
}

static void setup_game() {
	QueryPerformanceCounter(&frame_time);
	init_frame_time = (double)frame_time.QuadPart;
	current_level = &level_2;
	setup_level(&level_2, "level_2.txt", 40);
	init_level_setup();
}

static void editor(Input* input, float dt, const Levels& level) {
	for (int i = 0; i <= 9; i++) {
		if (pressed(i)) {
			colour_tiles = i;
		}
	}

	if (input->is_mouse_down) {
		level_edit_element(coords_to_tiles((int)input->mouse_x + camera_pos.X, 40),
			coords_to_tiles((int)input->mouse_y + camera_pos.Y, 40),
			colour_tiles,
			current_level);
	}

	if (pressed(BUTTON_P)) {
		write_level_into_file(*current_level, current_level->file_name);
	}

	if (pressed(BUTTON_W)) {
		change_level_editor_call(current_level, &return_next_level(*current_level));
	}

	if (colour_tiles)
		draw_tiles(coords_to_tiles((int)(input->mouse_x + camera_pos.X), 40),
			coords_to_tiles((int)(input->mouse_y + camera_pos.Y), 40),
			tile_id_to_colour(colour_tiles), level,
			camera_pos);
}


static void simulate_game(Input* input, float dt) {
	input->mouse_is_just_pressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	if (input->mouse_is_just_pressed && !input->mouse_was_just_pressed) {
		input->mouse_just_pressed = true;
	}
	else {
		input->mouse_just_pressed = false;
	}

	if (pressed(BUTTON_I)) {
		if (game_state == EDITOR)
			game_state = PLAYING;
		else if (game_state == PLAYING)
			game_state = EDITOR;
	}

	clear_screen(SKY_BLUE);
	QueryPerformanceCounter(&frame_time);
	double performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (double)perf.QuadPart;
	}

	cur_frame_time = (double)frame_time.QuadPart;

	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);

	render_static_tiles(*current_level, camera_pos);

	switch (game_state) {
		case EDITOR: {
			editor(input, dt, *current_level);
			editor_camera(input, 1000, dt);
			break;
		}
		case PLAYING: {
			player_camera(&player, dt);
			simulate_player(input, dt, *current_level, &player, camera_pos);
			check_level_change(current_level, &return_next_level(*current_level), &player);
			break;
		}
	}

	input->mouse_was_just_pressed = input->mouse_is_just_pressed;
}