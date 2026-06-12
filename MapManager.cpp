#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include "Headers/MapManager.hpp"
#include "Headers/Global.hpp"

MapManager::MapManager() : 
    animation_frame_counter(0)
{
}

bool MapManager::initialize()
{
    if (!load_textures())
    {
        return false;
    }

    if (!load_map_from_image("Resources/level_1.png"))
    {
        return false;
    }

    return true;
}

bool MapManager::load_textures()
{
    if (!tex_floor1.loadFromFile("Resources/block1.png")) return false;
    if (!tex_floor2.loadFromFile("Resources/block2.png")) return false;
    if (!tex_brick.loadFromFile("Resources/bookblock.png")) return false;
    if (!tex_platform.loadFromFile("Resources/block2.png")) return false;
    if (!tex_mail_block.loadFromFile("Resources/mailblock0.png")) return false;
    if (!tex_activated_mail.loadFromFile("Resources/mailblock4.png")) return false;
    if (!tex_ects.loadFromFile("Resources/ECTS1.png")) return false;

    return true;
}

bool MapManager::load_map_from_image(const std::string& i_path)
{
    sf::Image map_image;
    if (!map_image.loadFromFile(i_path))
    {
        return false;
    }

    sf::Vector2u image_size = map_image.getSize();
    
    current_map.clear();
    current_map.resize(image_size.x);

    for (unsigned int x = 0; x < image_size.x; x++)
    {
        // Wypełniamy całą kolumnę pustymi blokami
        current_map[x].fill(Cell::Empty);
        
        for (unsigned int y = 0; y < image_size.y && y < 16; y++)
        {
            sf::Color pixel_color = map_image.getPixel({x, y});
            Cell cell_type = Cell::Empty;

            if (pixel_color == sf::Color(136, 0, 21))
            {
                cell_type = Cell::Floor1; 
            }
            else if (pixel_color == sf::Color(237, 28, 36))
            {
                cell_type = Cell::Brick; 
            }
            else if (pixel_color == sf::Color(163, 73, 164))
            {
                cell_type = Cell::MailBlock; 
            }
            else if (pixel_color == sf::Color(255, 242, 0))
            {
                cell_type = Cell::Platform; 
            }
            else if (pixel_color == sf::Color(0, 162, 232))
            {
                cell_type = Cell::Floor2; 
            }

            current_map[x][y] = cell_type;
        }
    }

    return true;
}

void MapManager::update(unsigned int i_camera_x)
{
    (void)i_camera_x;

    // Przełączanie klatek animacji co 150 milisekund (zakres 0 do 3)
    if (animation_clock.getElapsedTime().asMilliseconds() > 150)
    {
        animation_frame_counter = (animation_frame_counter + 1) % 4; 
        animation_clock.restart();
    }
}

void MapManager::draw_map(bool i_background, bool i_foreground, unsigned int i_camera_x, sf::RenderWindow& i_window)
{
    (void)i_background;
    (void)i_foreground;

    // Pobieramy domyślną teksturę do poprawnej inicjalizacji sprite'a
    sf::Sprite tile_sprite(tex_floor1);
    tile_sprite.setScale(sf::Vector2f{SCREEN_RESIZE, SCREEN_RESIZE});

    unsigned int start_x = i_camera_x / CELL_SIZE;
    unsigned int end_x = (i_camera_x + SCREEN_WIDTH) / CELL_SIZE + 1;

    if (end_x > current_map.size()) 
    {
        end_x = current_map.size();
    }

    for (unsigned int x = start_x; x < end_x; x++)
    {
        for (unsigned int y = 0; y < 16; y++)
        {
            if (current_map[x][y] == Cell::Empty)
            {
                continue;
            }

            tile_sprite.setPosition(sf::Vector2f{
                static_cast<float>((x * CELL_SIZE - i_camera_x) * SCREEN_RESIZE),
                static_cast<float>(y * CELL_SIZE * SCREEN_RESIZE)
            });

            // Resetujemy wycinek tekstury do pełnego rozmiaru kafelka przed sprawdzeniem typu animacji
            tile_sprite.setTextureRect(sf::IntRect({0, 0}, {CELL_SIZE, CELL_SIZE}));

            switch (current_map[x][y])
            {
                case Cell::Floor1:
                    tile_sprite.setTexture(tex_floor1);
                    break;
                case Cell::Floor2:
                    tile_sprite.setTexture(tex_ects);
                    // Każda kolejna klatka animacji ECTS jest przesunięta w prawo o szerokość CELL_SIZE w pliku graficznym
                    tile_sprite.setTextureRect(sf::IntRect({static_cast<int>(animation_frame_counter * CELL_SIZE), 0}, {CELL_SIZE, CELL_SIZE}));
                    break;
                case Cell::Brick:
                    tile_sprite.setTexture(tex_brick);
                    break;
                case Cell::Platform:
                    tile_sprite.setTexture(tex_platform);
                    break;
                case Cell::MailBlock:
                    tile_sprite.setTexture(tex_mail_block);
                    // Przesuwamy wycinek tekstury w poziomie, aby uzyskać animację pulsowania koperty
                    tile_sprite.setTextureRect(sf::IntRect({static_cast<int>(animation_frame_counter * CELL_SIZE), 0}, {CELL_SIZE, CELL_SIZE}));
                    break;
                case Cell::ActivatedMailBlock:
                    tile_sprite.setTexture(tex_activated_mail);
                    break;
                default:
                    continue;
            }

            i_window.draw(tile_sprite);
        }
    }
}

unsigned int MapManager::get_map_width() const
{
    return current_map.size() * CELL_SIZE;
}