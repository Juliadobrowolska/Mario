#include <cmath>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"

void MapManager::draw_map(const bool i_draw_background, const bool i_draw_foreground, const unsigned int i_view_x, sf::RenderWindow& i_window)
{
    (void)i_draw_background;
    (void)i_draw_foreground;

    int map_start = static_cast<int>(std::floor(i_view_x / static_cast<float>(CELL_SIZE)));
    int map_end = static_cast<int>(std::ceil((i_view_x + SCREEN_WIDTH) / static_cast<float>(CELL_SIZE)));

    if (map_start < 0) map_start = 0;

    sf::Sprite cell_sprite(tex_floor1);
    const sf::IntRect tile_rect(sf::Vector2i{0, 0}, sf::Vector2i{CELL_SIZE, CELL_SIZE});
    cell_sprite.setTextureRect(tile_rect);

    for (int a = map_start; a < map_end; a++)
    {
        if (a >= static_cast<int>(get_map_width() / CELL_SIZE)) break;

        for (unsigned short b = 0; b < SCREEN_HEIGHT / CELL_SIZE; b++)
        {
            Cell current_cell = get_cell_at(static_cast<unsigned int>(a), b);
            if (Cell::Empty == current_cell) continue;

            cell_sprite.setPosition(sf::Vector2f(static_cast<float>(CELL_SIZE * a - i_view_x), static_cast<float>(CELL_SIZE * b)));
            cell_sprite.setTextureRect(tile_rect);

            if (Cell::Floor1 == current_cell)
            {
                cell_sprite.setTexture(tex_floor1);
            }
            else if (Cell::Floor2 == current_cell)
            {
                cell_sprite.setTexture(tex_floor2);
            }
            else if (Cell::Brick == current_cell)
            {
                cell_sprite.setTexture(tex_brick);
            }
            else if (Cell::Platform == current_cell)
            {
                cell_sprite.setTexture(tex_platform);
            }
            else if (Cell::MailBlock == current_cell)
            {
                cell_sprite.setTexture(tex_mail_block);
            }
            else if (Cell::ActivatedMailBlock == current_cell)
            {
                cell_sprite.setTexture(tex_activated_mail);
            }
            else
            {
                cell_sprite.setTexture(tex_floor1);
            }

            i_window.draw(cell_sprite);
        }
    }
}