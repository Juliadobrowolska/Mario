#include <algorithm>
#include <vector>
#include <cmath>

#include "Headers/Global.hpp"
#include "Headers/MapCollision.hpp"
#include "Headers/MapManager.hpp"

unsigned char map_collision(float i_x, float i_y, const std::vector<Cell>& i_check_cells, const MapManager& i_map_manager)
{
	float cell_x = i_x / CELL_SIZE;
	float cell_y = i_y / CELL_SIZE;

	unsigned char output = 0;

	// Iteracja przez 4 punkty skrajne (rogi) wokół podanych współrzędnych
	for (unsigned char a = 0; a < 4; a++)
	{
		short x;
		short y;

		switch (a)
		{
			case 0: // Lewy górny róg
			{
				x = floor(cell_x);
				y = floor(cell_y);
				break;
			}
			case 1: // Prawy górny róg
			{
				x = ceil(cell_x);
				y = floor(cell_y);
				break;
			}
			case 2: // Lewy dolny róg
			{
				x = floor(cell_x);
				y = ceil(cell_y);
				break;
			}
			case 3: // Prawy dolny róg
			{
				x = ceil(cell_x);
				y = ceil(cell_y);
				break;
			}
		}

		// Pobranie całkowitej szerokości mapy w przeliczeniu na kafelki
		unsigned short map_tile_width = i_map_manager.get_map_width() / CELL_SIZE;

		// Weryfikacja zakresu współrzędnych w granicach mapy
		if (0 <= x && x < map_tile_width)
		{
			if (0 <= y && y < 16) // Sztywna wysokość segmentu (16 kafelków)
			{
				// Pobranie typu komórki z obiektu MapManager
				Cell current_cell = i_map_manager.get_cell_at(x, y);

				// Sprawdzenie obecności pobranej komórki w wektorze obiektów kolizyjnych
				if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), current_cell))
				{
					output += pow(2, a);
				}
			}
		}
		// Traktowanie granic mapy jako obiektów stałych (ścian) przy wyjściu poza zakres
		else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
		{
			output += pow(2, a);
		}
	}

	return output;
}