#define is_down(b) input->button[b].is_down
#define pressed(b) (input->button[b].is_down && input->button[b].changed)
#define relased(b) (!input->button[b].is_down && input->button[b].changed)

static Vector2 camera_pos;

static float smoothing = 5.f;

static float horizontal_offset = 200.f;

static void player_camera(Player* player, float dt) {
	float intended_camera_x = player->player_position().X - (drawable_area.width / 2) + (player->player_direction() * horizontal_offset);
	float intended_camera_y = player->player_position().Y - (drawable_area.height / 2);
	camera_pos.X += (intended_camera_x - camera_pos.X) * smoothing * dt;
	camera_pos.Y += (intended_camera_y - camera_pos.Y) * smoothing * dt;
}

static void camera_reset(Player* player) {
	camera_pos.X = player->player_position().X - (drawable_area.width / 2) + (player->player_direction() * horizontal_offset);
	camera_pos.Y = player->player_position().Y - (drawable_area.height / 2);
}

static void camera_reset_editor() {
	camera_pos.X = 0;
	camera_pos.Y = 0;
}

static void editor_camera(Input* input, int speed, float dt) {
	if (is_down(BUTTON_LEFT)) {
		camera_pos.X -= speed * dt;
	}
	if (is_down(BUTTON_RIGHT)) {
		camera_pos.X += speed * dt;
	}
	if (is_down(BUTTON_UP)) {
		camera_pos.Y -= speed * dt;
	}
	if (is_down(BUTTON_DOWN)) {
		camera_pos.Y += speed * dt;
	}
}