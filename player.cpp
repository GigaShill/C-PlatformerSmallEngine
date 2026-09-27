#define is_down(b) input->button[b].is_down
#define pressed(b) (input->button[b].is_down && input->button[b].changed)
#define relased(b) (!input->button[b].is_down && input->button[b].changed)

#include <algorithm>

void Player::render(const Vector2& camera_position) {
	draw_player((int)(position.X - camera_position.X), (int)(position.Y - camera_position.Y), half_size * 2, PINK);
}

void Player::reset() {
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

void Player::move_and_collide(Input* input, float dt, const Levels& level) {

	if (is_down(BUTTON_D)) {
		direction.X = 1;
		facing_direction = 1;
	}
	else if (is_down(BUTTON_A)) {
		direction.X = -1;
		facing_direction = -1;
	}
	else {
		direction.X = 0;
	}

	if (is_down(BUTTON_W) && is_on_ground) {
		velocity.Y = -800.f;;
	}

	if(!is_on_ground) {
		if (velocity.Y < 0)
			velocity.Y += gravity * dt;
		else
			velocity.Y += (gravity + 500.f) * dt;
	}

	velocity.X = direction.X * speed;

	

	position.X += (velocity.X * dt);
	collide_axis(level, true);
	position.Y += (velocity.Y * dt);
	collide_axis(level, false);

	is_on_ground_probe(level);
}

void Player::collide_axis(const Levels& level, bool resolve_x) {
	struct AABB {
		float min_x, min_y, max_x, max_y;
	};

	AABB player_box = {
		position.X - half_size, position.Y - half_size,
		position.X + half_size, position.Y + half_size
	};

	int min_tile_x = coords_to_tiles((int)player_box.min_x, size);
	int max_tile_x = coords_to_tiles((int)player_box.max_x, size);
	int min_tile_y = coords_to_tiles((int)player_box.min_y, size);
	int max_tile_y = coords_to_tiles((int)player_box.max_y, size);

	float correction = 0;
	bool collided = false;

	for (int ty = min_tile_y; ty <= max_tile_y; ty++) {
		for (int tx = min_tile_x; tx <= max_tile_x; tx++) {
			if (level_get_tile(level, tx, ty) == 0)
				continue;

			float tile_min_x = tiles_to_coords(tx, size);
			float tile_min_y = tiles_to_coords(ty, size);
			float tile_max_x = tile_min_x + size;
			float tile_max_y = tile_min_y + size;

			float overlap_x = (std::min)(player_box.max_x, tile_max_x) - (std::max)(player_box.min_x, tile_min_x);
			float overlap_y = (std::min)(player_box.max_y, tile_max_y) - (std::max)(player_box.min_y, tile_min_y);

			if (overlap_x <= 0 || overlap_y <= 0)
				continue;

			if (resolve_x) {
				float push = (player_box.min_x < tile_min_x) ? -overlap_x : overlap_x;
				if (std::abs(push) > std::abs(correction))
					correction = push;
				collided = true;
			}
			else {
				float push = (player_box.min_y < tile_min_y) ? -overlap_y : overlap_y;
				if (std::abs(push) > std::abs(correction))
					correction = push;
				collided = true;
			}
		}
	}

	if (resolve_x && collided) {
		position.X += correction;
		velocity.X = 0;
	}
	else if (!resolve_x && collided) {
		position.Y += correction;
		velocity.Y = 0;
		if (correction > 0)
			is_on_ground = true;
	}
}

void Player::is_on_ground_probe(const Levels& level) {
	const float ground_check_tolerance = 0.5f;

	struct AABB {
		float min_x, min_y, max_x, max_y;
	};

	AABB player_box = {
		position.X - half_size, position.Y - half_size,
		position.X + half_size, position.Y + half_size
	};

	int min_tile_x = coords_to_tiles((int)player_box.min_x, size);
	int max_tile_x = coords_to_tiles((int)player_box.max_x - 5, size);

	float probe_min_y = player_box.max_y;
	float probe_max_y = player_box.max_y + ground_check_tolerance;

	int probe_tile_min_y = coords_to_tiles((int)probe_min_y, size);
	int probe_tile_max_y = coords_to_tiles((int)probe_max_y, size);

	is_on_ground = false;

	for (int ty = probe_tile_min_y; ty <= probe_tile_max_y; ty++) {
		for (int tx = min_tile_x; tx <= max_tile_x; tx++) {
			if (level_get_tile(level, tx, ty) != 0) {
				is_on_ground = true;
			}
		}
	}
}

static void simulate_player(Input* input, float dt, const Levels &level, Player* player, const Vector2& camera_position) {
	player->move_and_collide(input, dt, level);
	player->render(camera_position);
}

Vector2 Player::player_position() {
	return player.position;
}

int Player::player_direction() {
	return facing_direction;
}