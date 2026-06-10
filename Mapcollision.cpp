#include <algorithm>
#include <vector>
#include <cmath>

#include "Headers/Global.hpp"
#include "Headers/MapCollision.hpp"
#include "Headers/MapManager.hpp"

unsigned char map_collision(float i_x, float i_y, const std::vector<Cell>& i_check_cells, const MapManager& i_map_manager)
{
    unsigned char output = 0;

    // Wyznaczenie pozycji krawędzi obiektu na podstawie rozmiaru CELL_SIZE
    float left_x = i_x;
    float right_x = i_x + CELL_SIZE - 1.0f;
    float top_y = i_y;
    float bottom_y = i_y + CELL_SIZE - 1.0f;

    // Pobranie całkowitej szerokości mapy w przeliczeniu na kafelki
    unsigned short map_tile_width = i_map_manager.get_map_width() / CELL_SIZE;

    // Iteracja przez 4 punkty skrajne (rogi) wokół podanych współrzędnych
    for (unsigned char a = 0; a < 4; a++)
    {
        short x = 0;
        short y = 0;

        switch (a)
        {
            case 0: // Lewy górny róg
            {
                x = static_cast<short>(std::floor(left_x / CELL_SIZE));
                y = static_cast<short>(std::floor(top_y / CELL_SIZE));
                break;
            }
            case 1: // Prawy górny róg
            {
                x = static_cast<short>(std::floor(right_x / CELL_SIZE));
                y = static_cast<short>(std::floor(top_y / CELL_SIZE));
                break;
            }
            case 2: // Lewy dolny róg
            {
                x = static_cast<short>(std::floor(left_x / CELL_SIZE));
                y = static_cast<short>(std::floor(bottom_y / CELL_SIZE));
                break;
            }
            case 3: // Prawy dolny róg
            {
                x = static_cast<short>(std::floor(right_x / CELL_SIZE));
                y = static_cast<short>(std::floor(bottom_y / CELL_SIZE));
                break;
            }
        }

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
                    // Użycie operacji bitowej zamiast funkcji pow()
                    output += (1 << a);
                }
            }
        }
        // Traktowanie granic mapy jako obiektów stałych (ścian) przy wyjściu poza zakres
        else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
        {
            // Użycie operacji bitowej zamiast funkcji pow()
            output += (1 << a);
        }
    }

    return output;
}