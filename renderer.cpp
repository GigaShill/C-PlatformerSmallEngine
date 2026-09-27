//Refreshes the screen background based on a colour
static void clear_screen(unsigned int colour) {
	//Set a unsigned int pointer for the pixels
	unsigned int* pixel = (unsigned int*)render_state.memory;
	//Iterates through each row and column
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			//Pointer arithmetic, increments by sizeof(unsigned int) bytes (usually 4)
			*pixel++ = colour;
		}
	}
}

//Draws rectangles using screen coords
static void draw_rect_in_pixels(int x0, int y0, int x1, int y1, unsigned int colour) {
	//Limit x and y to the boundaries of the window
	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);

	//Iterates through each row and column
	for (int y = y0; y < y1; y++) {
		//Set a unsigned int pointer for the pixels
		unsigned int* pixel = (unsigned int*)render_state.memory + x0 + y * render_state.width;
		for (int x = x0; x < x1; x++) {
			//Pointer arithmetic, increments by one unsigned int
			*pixel++ = colour;
		}
	}
}

//Scale to render without screen coords
static float render_scale = 0.0001f;

//Draws rect without screen coords (0,0 is centre)
static void draw_rect(float x, float y, float half_size_x, float half_size_y, unsigned int colour) {
	x *= render_state.height * render_scale;
	y *= render_state.height * render_scale;
	half_size_x *= render_state.height * render_scale;
	half_size_y *= render_state.height * render_scale;

	x += render_state.width / 2.f;
	y += render_state.height / 2.f;
	
	//Change to pixels
	int x0 = x - half_size_x;
	int x1 = x + half_size_x;
	int y0 = y - half_size_y;
	int y1 = y + half_size_y;

	draw_rect_in_pixels(x0, y0, x1, y1, colour);
}

//Draw numbers painstakingly by hand
static void draw_number(int number, float x, float y, float size, unsigned int colour) {
	float half_size = size * 0.5f;

	bool drew_number = false;
	while (number || !drew_number) {
		//Exists cause we want to draw zeroes as well
		drew_number = true;
		//Go digit by digit
		int digit = number % 10;
		//next digit
		number /= 10;

		//Draw individual digit and offset each time for next
		switch (digit) {
		case 0: {
			draw_rect(x - size, y, half_size, 2.5f * size, colour);
			draw_rect(x + size, y, half_size, 2.5f * size, colour);
			draw_rect(x, y + size * 2.f, half_size, half_size, colour);
			draw_rect(x, y - size * 2.f, half_size, half_size, colour);
			x -= size * 4.f;
		} break;

		case 1: {
			draw_rect(x + size, y, half_size, 2.5f * size, colour);
			x -= size * 2.f;
		} break;

		case 2: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, colour);
			draw_rect(x, y, 1.5f * size, half_size, colour);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, colour);
			draw_rect(x + size, y + size, half_size, half_size, colour);
			draw_rect(x - size, y - size, half_size, half_size, colour);
			x -= size * 4.f;
		} break;

		case 3: {
			draw_rect(x - half_size, y + size * 2.f, size, half_size, colour);
			draw_rect(x - half_size, y, size, half_size, colour);
			draw_rect(x - half_size, y - size * 2.f, size, half_size, colour);
			draw_rect(x + size, y, half_size, 2.5f * size, colour);
			x -= size * 4.f;
		} break;

		case 4: {
			draw_rect(x + size, y, half_size, 2.5f * size, colour);
			draw_rect(x - size, y + size, half_size, 1.5f * size, colour);
			draw_rect(x, y, half_size, half_size, colour);
			x -= size * 4.f;
		} break;

		case 5: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, colour);
			draw_rect(x, y, 1.5f * size, half_size, colour);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, colour);
			draw_rect(x - size, y + size, half_size, half_size, colour);
			draw_rect(x + size, y - size, half_size, half_size, colour);
			x -= size * 4.f;
		} break;

		case 6: {
			draw_rect(x + half_size, y + size * 2.f, size, half_size, colour);
			draw_rect(x + half_size, y, size, half_size, colour);
			draw_rect(x + half_size, y - size * 2.f, size, half_size, colour);
			draw_rect(x - size, y, half_size, 2.5f * size, colour);
			draw_rect(x + size, y - size, half_size, half_size, colour);
			x -= size * 4.f;
		} break;

		case 7: {
			draw_rect(x + size, y, half_size, 2.5f * size, colour);
			draw_rect(x - half_size, y + size * 2.f, size, half_size, colour);
			x -= size * 4.f;
		} break;

		case 8: {
			draw_rect(x - size, y, half_size, 2.5f * size, colour);
			draw_rect(x + size, y, half_size, 2.5f * size, colour);
			draw_rect(x, y + size * 2.f, half_size, half_size, colour);
			draw_rect(x, y - size * 2.f, half_size, half_size, colour);
			draw_rect(x, y, half_size, half_size, colour);
			x -= size * 4.f;
		} break;

		case 9: {
			draw_rect(x - half_size, y + size * 2.f, size, half_size, colour);
			draw_rect(x - half_size, y, size, half_size, colour);
			draw_rect(x - half_size, y - size * 2.f, size, half_size, colour);
			draw_rect(x + size, y, half_size, 2.5f * size, colour);
			draw_rect(x - size, y + size, half_size, half_size, colour);
			x -= size * 4.f;
		} break;
		}
	}
}

//Tiles dont move and we want to render them from the level
static void render_static_tiles(const Levels &level, const Vector2& camera_position) {
	//Iterate through width and height
	for (int i = 0; i < level.width; i++) {
		for (int j = 0; j < level.height; j++) {
			//If a tile exists (tile != 0), draw it, the - camera_pos to account camera movement
			if(level_get_tile(level, i, j) != 0)
				draw_rect_in_pixels(tiles_to_coords(i, level.tile_size) - camera_position.X, 
					tiles_to_coords(j, level.tile_size) - camera_position.Y,
					tiles_to_coords(i, level.tile_size) + level.tile_size - camera_position.X,
					tiles_to_coords(j, level.tile_size) + level.tile_size - camera_position.Y,
					//The colour of tile based on if we read 1-9
					tile_id_to_colour(level_get_tile(level, i, j)));
		}
	}
}

//Draw individual tiles
static void draw_tiles(int x, int y, unsigned int colour, const Levels& level, const Vector2& camera_position) {
	draw_rect_in_pixels(tiles_to_coords(x, level.tile_size) - camera_position.X,
		tiles_to_coords(y, level.tile_size) - camera_position.Y,
		tiles_to_coords(x, level.tile_size) + level.tile_size - camera_position.X,
		tiles_to_coords(y, level.tile_size) + level.tile_size - camera_position.Y, colour);
}

//Draw individual pixels
static void draw_pixels(int x, int y, unsigned int colour) {
	draw_rect_in_pixels(tiles_to_coords(x, 1), tiles_to_coords(y, 1), tiles_to_coords(x, 1) + 1, tiles_to_coords(y, 1) + 1, colour);
}

//Draw player sprite
static void draw_player(int x, int y, int size, unsigned int colour) {
	int x0 = x - (size / 2);
	int y0 = y - (size / 2);
	int x1 = x + (size / 2);
	int y1 = y + (size / 2);
	int p_size = size / 10;
	//Change sprite based on direction
	if(player.player_direction() == 1) {
		draw_rect_in_pixels(x0, y0, x1, y0 + p_size, BLACK);
		draw_rect_in_pixels(x0, y1 - p_size, x1, y1, BLACK);
		draw_rect_in_pixels(x0, y0, x0 + p_size, y1, BLACK);
		draw_rect_in_pixels(x1 - p_size, y0, x1, y1, BLACK);
		draw_rect_in_pixels(x0 + p_size, y0 + p_size, x0 + (6 * p_size), y0 + (9 * p_size), colour);
		draw_rect_in_pixels(x0 + (6 * p_size), y0 + p_size, x0 + (9 * p_size), y0 + (3 * p_size), colour);
		draw_rect_in_pixels(x0 + (6 * p_size), y0 + (5 * p_size), x0 + (9 * p_size), y0 + (9 * p_size), colour);
		draw_rect_in_pixels(x0 + (8 * p_size), y0 + (3 * p_size), x0 + (9 * p_size), y0 + (5 * p_size), colour);
		draw_rect_in_pixels(x0 + (6 * p_size), y0 + (3 * p_size), x0 + (7 * p_size), y0 + (4 * p_size), BLACK);
		draw_rect_in_pixels(x0 + (7 * p_size), y0 + (3 * p_size), x0 + (8 * p_size), y0 + (4 * p_size), WHITE);
		draw_rect_in_pixels(x0 + (6 * p_size), y0 + (4 * p_size), x0 + (8 * p_size), y0 + (5 * p_size), BLACK);
	}
	else if(player.player_direction() == -1) {
		draw_rect_in_pixels(x0, y0, x1, y0 + p_size, BLACK);
		draw_rect_in_pixels(x0, y1 - p_size, x1, y1, BLACK);
		draw_rect_in_pixels(x0, y0, x0 + p_size, y1, BLACK);
		draw_rect_in_pixels(x1 - p_size, y0, x1, y1, BLACK);
		draw_rect_in_pixels(x1 - (6 * p_size), y0 + p_size, x1 - p_size, y0 + (9 * p_size), colour);
		draw_rect_in_pixels(x1 - (9 * p_size), y0 + p_size, x1 - (6 * p_size), y0 + (3 * p_size), colour);
		draw_rect_in_pixels(x1 - (9 * p_size), y0 + (5 * p_size), x1 - (6 * p_size), y0 + (9 * p_size), colour);
		draw_rect_in_pixels(x1 - (9 * p_size), y0 + (3 * p_size), x1 - (8 * p_size), y0 + (5 * p_size), colour);
		draw_rect_in_pixels(x1 - (7 * p_size), y0 + (3 * p_size), x1 - (6 * p_size), y0 + (4 * p_size), BLACK);
		draw_rect_in_pixels(x1 - (8 * p_size), y0 + (3 * p_size), x1 - (7 * p_size), y0 + (4 * p_size), WHITE);
		draw_rect_in_pixels(x1 - (8 * p_size), y0 + (4 * p_size), x1 - (6 * p_size), y0 + (5 * p_size), BLACK);
	}
}