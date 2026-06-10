#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <iostream>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"

MapManager::MapManager() :
    animation_frame(0)
{
}

bool MapManager::initialize()
{
    if (!load_textures())
    {
        std::cout << "Error: Failed to load map textures." << std::endl;
        return false;
    }

    if (!load_map_from_image("Resources/level_1.png"))
    {
        std::cout << "Warning: Failed to load map image, using fallback map." << std::endl;
    }

    return true;
}

bool MapManager::load_textures()
{
    // Reallocation of textures based on the new visual design layers
    if (!tex_floor1.loadFromFile("Resources/block2.png")) return false;       // Burgundy underlayment
    if (!tex_floor2.loadFromFile("Resources/block1.png")) return false;       // Alternative block element
    if (!tex_brick.loadFromFile("Resources/block1.png")) return false;        // Red breakable blocks
    if (!tex_platform.loadFromFile("Resources/bookblock.png")) return false;   // Yellow book blocks
    if (!tex_mail_block.loadFromFile("Resources/mailblock0.png")) return false;// Purple bonus box
    if (!tex_activated_mail.loadFromFile("Resources/mailblock4.png")) return false;
    return true;
}

bool MapManager::load_map_from_image(const std::string& i_path)
{
    sf::Image map_image;
    if (!map_image.loadFromFile(i_path))
    {
        current_map.clear();
        const unsigned int map_width = 16;
        current_map.resize(map_width);

        for (unsigned int x = 0; x < map_width; x++)
        {
            current_map[x].fill(Cell::Empty);
            current_map[x][(SCREEN_HEIGHT / CELL_SIZE) - 1] = Cell::Floor1;
        }

        return true;
    }

    sf::Vector2u image_size = map_image.getSize();
    current_map.clear();
    current_map.resize(image_size.x);

    for (unsigned int x = 0; x < image_size.x; x++)
    {
        current_map[x].fill(Cell::Empty);
        for (unsigned int y = 0; y < image_size.y && y < SCREEN_HEIGHT / CELL_SIZE; y++)
        {
            sf::Color pixel_color = map_image.getPixel({x, y});
            Cell cell_type = Cell::Empty;

            // Mapping pixels based directly on the newly specified game logic rules
            if (pixel_color == sf::Color(136, 0, 21))
            {
                cell_type = Cell::Floor1;      // Burgundy: main bottom floor layout
            }
            else if (pixel_color == sf::Color(237, 28, 36))
            {
                cell_type = Cell::Brick;       // Red: common destructible bricks
            }
            else if (pixel_color == sf::Color(163, 73, 164))
            {
                cell_type = Cell::MailBlock;   // Purple: item box container
            }
            else if (pixel_color == sf::Color(255, 242, 0))
            {
                cell_type = Cell::Platform;    // Yellow: book stack platforms
            }
            else if (pixel_color == sf::Color(0, 162, 232))
            {
                // Temporary assignment for the blue ECTS token container placeholder
                cell_type = Cell::Floor2;      
            }

            current_map[x][y] = cell_type;
        }
    }

    return true;
}

void MapManager::load_level(const std::string& i_level_path)
{
    load_map_from_image(i_level_path);
}

void MapManager::update(const unsigned int i_view_x)
{
    (void)i_view_x;
}

unsigned int MapManager::get_map_width() const
{
    return static_cast<unsigned int>(current_map.size()) * CELL_SIZE;
}

Cell MapManager::get_cell_at(unsigned int i_x, unsigned int i_y) const
{
    if (i_x >= current_map.size() || i_y >= SCREEN_HEIGHT / CELL_SIZE)
    {
        return Cell::Empty;
    }
    return current_map[i_x][i_y];
}