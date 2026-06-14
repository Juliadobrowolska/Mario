#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
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
    
    // Ładowanie pliku poziomu
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
            sf::Color pixel = level_image.getPixel(sf::Vector2u{static_cast<unsigned int>(x), static_cast<unsigned int>(y)});

            // 1. Czarny (0, 0, 0) -> Stałe podłoże
            if (pixel == sf::Color(0, 0, 0))
            {
                map_grid[x][y] = Cell::Wall;
            }
            // 2. Czerwony (237, 28, 36) -> Wiszące klocki (Brick)
            else if (pixel == sf::Color(237, 28, 36))
            {
                map_grid[x][y] = Cell::Brick;
            }
            // 3. Bordowy (136, 0, 21) -> Dolna podłoga (traktowana jako ściana)
            else if (pixel == sf::Color(136, 0, 21))
            {
                map_grid[x][y] = Cell::Wall;
            }
            // 4. Żółty (255, 242, 0) -> Platformy z książkami
            else if (pixel == sf::Color(255, 242, 0))
            {
                map_grid[x][y] = Cell::Platform;
            }
            // 5. Fioletowy (163, 73, 164) -> Skrzynki pocztowe
            else if (pixel == sf::Color(163, 73, 164))
            {
                map_grid[x][y] = Cell::MailBlock;
            }
            // 6. Jasnoniebieski (0, 162, 232) -> Spawny przeciwników
            else if (pixel == sf::Color(0, 162, 232))
            {
                unsigned char enemy_type = (std::rand() % 2);
                enemy_positions.push_back({enemy_type, static_cast<float>(x * CELL_SIZE), static_cast<float>(y * CELL_SIZE)});
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
}

std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox) const
{
    std::vector<unsigned char> output;

    short start_x = std::max(0, static_cast<int>(std::floor(i_hitbox.position.x / CELL_SIZE)));
    short end_x = std::min(static_cast<int>(map_width - 1), static_cast<int>(std::ceil((i_hitbox.position.x + i_hitbox.size.x) / CELL_SIZE)));
    short start_y = std::max(0, static_cast<int>(std::floor(i_hitbox.position.y / CELL_SIZE)));
    short end_y = std::min(static_cast<int>(map_height - 1), static_cast<int>(std::ceil((i_hitbox.position.y + i_hitbox.size.y) / CELL_SIZE)));

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