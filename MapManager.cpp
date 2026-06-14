#include <vector>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"

MapManager::MapManager() :
	map_width(0),
	map_height(0)
{
}

bool MapManager::load_level(unsigned short i_level_number)
{
	sf::Image level_image;
	
	// DOPASOWANIE DO STRUKTURY: Ładowanie plików level_1, level_2, level_3 bez dodatkowych dopisków
	if (!level_image.loadFromFile("Resources/level_" + std::to_string(i_level_number) + ".png"))
	{
		return false;
	}

	map_width = level_image.getSize().x;
	map_height = level_image.getSize().y;

	map_grid.assign(map_width, std::vector<Cell>(map_height, Cell::Empty));
	enemy_positions.clear();

	for (unsigned short x = 0; x < map_width; x++)
	{
		for (unsigned short y = 0; y < map_height; y++)
		{
			sf::Color pixel = level_image.getPixel(x, y);

			// KOLORY I ICH POWIĄZANIE Z TWOIMI GRAFIKAMI:
			// Czarny piksel to podłoga/ściany (block1.png / block2.png)
			if (pixel == sf::Color::Black)
			{
				map_grid[x][y] = Cell::Wall;
			}
			// Czerwony piksel to niszczalny klocek (w main podpięty pod block1.png)
			else if (pixel == sf::Color::Red)
			{
				map_grid[x][y] = Cell::Brick;
			}
			// Niebieski piksel to półka na książki (bookblock.png)
			else if (pixel == sf::Color::Blue)
			{
				map_grid[x][y] = Cell::Platform;
			}
			// Fioletowy (Magenta) piksel to pytajnik z zadaniem (mailblock3.png)
			else if (pixel == sf::Color(255, 0, 255))
			{
				map_grid[x][y] = Cell::MailBlock;
			}
			
			// POZYCJE STARTOWE WROGÓW:
			// Zielony piksel = Miejsce startowe dla grade2 (Oceny 2.0)
			else if (pixel == sf::Color(0, 255, 0))
			{
				enemy_positions.push_back({0, static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE)});
			}
			// Żółty piksel = Miejsce startowe dla profesora wybiegającego z walizki
			else if (pixel == sf::Color(255, 255, 0))
			{
				enemy_positions.push_back({1, static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE)});
			}
		}
	}

	return true;
}

Cell MapManager::get_cell(unsigned short i_x, unsigned short i_y) const
{
	if (i_x >= map_width || i_y >= map_height)
	{
		return Cell::Empty;
	}
	return map_grid[i_x][i_y];
}

void MapManager::set_cell(unsigned short i_x, unsigned short i_y, Cell i_cell)
{
	if (i_x < map_width && i_y < map_height)
	{
		map_grid[i_x][i_y] = i_cell;
	}
}

unsigned short MapManager::get_map_width() const
{
	return map_width;
}

const std::vector<MapManager::EnemySpawnPoint>& MapManager::get_enemy_positions() const
{
	return enemy_positions;
}

void MapManager::add_ects()
{
	// Logika punktów zsynchronizowana bezpośrednio w main.cpp na podstawie zebranych Energydrink
}

std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox) const
{
	std::vector<unsigned char> output;

	short start_x = std::max(0, static_cast<int>(floor(i_hitbox.left / CELL_SIZE)));
	short end_x = std::min(static_cast<int>(map_width - 1), static_cast<int>(ceil((i_hitbox.left + i_hitbox.width) / CELL_SIZE)));
	short start_y = std::max(0, static_cast<int>(floor(i_hitbox.top / CELL_SIZE)));
	short end_y = std::min(static_cast<int>(map_height - 1), static_cast<int>(ceil((i_hitbox.top + i_hitbox.height) / CELL_SIZE)));

	for (short x = start_x; x < end_x; x++)
	{
		for (short y = start_y; y < end_y; y++)
		{
			Cell current_cell = get_cell(x, y);
			
			if (std::find(i_check_cells.begin(), i_check_cells.end(), current_cell) != i_check_cells.end())
			{
				output.push_back(1);
			}
		}
	}

	if (output.empty())
	{
		output.push_back(0);
	}

	return output;
}