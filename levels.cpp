#include <fstream>
#include <iostream>

Levels level_1;
Levels level_2;

Levels* current_level;

static void level_create(int width, int height, void** memory, int size) {
	*memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	for (int i = 0; i < height; i++) {
		unsigned int* tile = (unsigned int*)(*memory) + i * width;
		for (int j = 0; j < width; j++) {
			*tile++ = 0;
		}
	}
}

static void level_create_and_edit(const char* file, Levels* level) {
	std::ifstream level_file;
	level_file.open(file);
	if (!level_file.is_open())
		return;

	std::string line;
	int file_width = 0, file_height = 0;
	while (std::getline(level_file, line)) {
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		file_width = line.length();
		file_height++;
	}

	level->width = file_width;
	level->height = file_height;
	level->size = level->width * level->height * sizeof(unsigned int);

	level_create(level->width, level->height, &level->memory, level->size);

	level_file.clear();
	level_file.seekg(0);

	int elem_h = 0;
	while (std::getline(level_file, line)) {
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		for (int i = 0; i < file_width; i++) {
			*((unsigned int*)level->memory + (elem_h * level->width) + i) = line[i] - '0';
		}
		elem_h++;
	}
}

static void level_edit_element(int x, int y, unsigned int tile_colour, Levels* level) {

	if(x < level->width && x >= 0 && y < level->height && y >= 0)
		*((unsigned int*)level->memory + (y * level->width) + x) = tile_colour;
}

static unsigned int level_get_tile(const Levels& level, int x, int y) {
	if (x < 0 || x >= level.width || y < 0 || y >= level.height)
		return 0;
	return *((unsigned int*)level.memory + (y * level.width) + x);
}

static void write_level_into_file(const Levels& level, const char* file) {
	std::ofstream level_file;
	level_file.open(file);
	if (!level_file.is_open())
		return;

	for (int i = 0; i < level.height; i++) {
		for (int j = 0; j < level.width; j++) {
			level_file << level_get_tile(level, j, i);
		}
		level_file << "\n";
	}

	level_file.close();
}

static void create_empty_file(const char* file) {
	std::ofstream level_file;
	level_file.open(file);
	if (!level_file.is_open())
		return;

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			if(i == 14 && j < 14)
				level_file << "4";
			else
				level_file << "0";
		}
		level_file << "\n";
	}

	level_file.close();
}

static void level_free_data(Levels* level) {
	VirtualFree(level->memory, 0, MEM_RELEASE);
}

static void setup_level(Levels* level, const char* file_name, int t_size) {
	level->tile_size = t_size;
	level->file_name = file_name;
	bool does_file_exist = false;
	std::ifstream level_file;
	level_file.open(level->file_name);
	does_file_exist = level_file.is_open();
	if (does_file_exist)
		level_create_and_edit(level->file_name, level);
	else {
		create_empty_file(level->file_name);
		level_create_and_edit(level->file_name, level);
	}
}

static void init_level_setup() {
	level_1.file_name = "level_1.txt";
	level_2.file_name = "level_2.txt";
}

static Levels& return_next_level(Levels& current_level) {

	if (&current_level == &level_1)
		return level_2;
	else if (&current_level == &level_2)
		return level_1;

	return level_1;
}