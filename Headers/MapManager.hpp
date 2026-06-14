#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Global.hpp"

class MapManager
{
public:
	struct EnemySpawnPoint
	{
		unsigned char type; // 0 = grade2, 1 = profesor
		float x;
		float y;
	};

	MapManager();

	bool load_level(unsigned short i_level_number);
	void set_cell(unsigned short i_x, unsigned short i_y, Cell i_cell);
	
	Cell get_cell(unsigned short i_x, unsigned short i_y) const;
	unsigned short get_map_width() const;
	const std::vector<EnemySpawnPoint>& get_enemy_positions() const;
	void add_ects();

	std::vector<unsigned char> map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox) const;

private:
	unsigned short map_width;
	unsigned short map_height;
	std::vector<std::vector<Cell>> map_grid;
	std::vector<EnemySpawnPoint> enemy_positions;
};