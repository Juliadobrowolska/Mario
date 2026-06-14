#include <array>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <random>
#include <string>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"

MapManager::MapManager() :
    animation_frame_mail(0),
    animation_frame_ects(0),
    current_level(1)
{
    // Ładowanie tekstur statycznych zgodnie z nazewnictwem użytkownika
    texture_floor1.loadFromFile("Resources/Images/block1.png");
    texture_brick.loadFromFile("Resources/Images/block2.png");
    texture_platform.loadFromFile("Resources/Images/bookblock.png");
    texture_activated_mail.loadFromFile("Resources/Images/mailblock4.png");

    // Ładowanie animacji skrzynki pocztowej (mailblock0 do mailblock3)
    for (int i = 0; i < 4; i++)
    {
        textures_mail_block[i].loadFromFile("Resources/Images/mailblock" + std::to_string(i) + ".png");
    }
    // Ostatnia klatka używana jako alternatywa
    textures_mail_block[4] = texture_activated_mail;

    // Ładowanie animacji punktów ECTS (ECTS1 do ECTS7)
    for (int i = 0; i < 7; i++)
    {
        textures_ects[i].loadFromFile("Resources/Images/ECTS" + std::to_string(i + 1) + ".png");
    }

    // Inicjalizacja generatora liczb losowych
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    random_engine.seed(seed);
    
    init_level_system();
}

void MapManager::init_level_system()
{
    current_level = 1;
    
    available_levels.clear();
    available_levels.push_back(2);
    available_levels.push_back(3);
    
    load_current_level_sketch();
}

void MapManager::load_next_random_level()
{
    if (available_levels.empty())
    {
        available_levels.push_back(1);
        available_levels.push_back(2);
        available_levels.push_back(3);
    }

    std::uniform_int_distribution<size_t> distribution(0, available_levels.size() - 1);
    size_t random_index = distribution(random_engine);
    
    current_level = available_levels[random_index];
    available_levels.erase(available_levels.begin() + random_index);
    
    load_current_level_sketch();
}

void MapManager::load_current_level_sketch()
{
    // Dostosowanie nazw plików do poziomu użytkownika (level_1.png, level_2.png, level_3.png)
    map_sketch.loadFromFile("Resources/Images/level_" + std::to_string(static_cast<unsigned short>(current_level)) + ".png");
    
    set_map_size(map_sketch.getSize().x);
    const unsigned int max_height = SCREEN_HEIGHT / CELL_SIZE;

    for (unsigned int x = 0; x < map_sketch.getSize().x; x++)
    {
        for (unsigned int y = 0; y < map_sketch.getSize().y && y < max_height; y++)
        {
            sf::Color pixel_color = map_sketch.getPixel({x, y});
            Cell cell_type = Cell::Empty;

            if (pixel_color == sf::Color(136, 0, 21))          cell_type = Cell::Floor1; 
            else if (pixel_color == sf::Color(237, 28, 36))     cell_type = Cell::Brick; 
            else if (pixel_color == sf::Color(163, 73, 164))   cell_type = Cell::MailBlock; 
            else if (pixel_color == sf::Color(255, 242, 0))     cell_type = Cell::Platform; 
            else if (pixel_color == sf::Color(0, 162, 232))     cell_type = Cell::Ects;

            map[x][y] = cell_type;
        }
    }
}

unsigned short MapManager::get_map_sketch_height() const
{
    return map_sketch.getSize().y;
}

unsigned short MapManager::get_map_sketch_width() const
{
    return map_sketch.getSize().x;
}

unsigned short MapManager::get_map_width() const
{
    return map.size() * CELL_SIZE;
}

void MapManager::add_brick_particles(const unsigned short i_x, const unsigned short i_y)
{
    brick_particles.push_back(Object(i_x, i_y, -0.25f * BRICK_PARTICLE_SPEED, -1.5f * BRICK_PARTICLE_SPEED));
    brick_particles.push_back(Object(i_x + 0.5f * CELL_SIZE, i_y, 0.25f * BRICK_PARTICLE_SPEED, -1.5f * BRICK_PARTICLE_SPEED));
    brick_particles.push_back(Object(i_x, i_y + 0.5f * CELL_SIZE, -0.5f * BRICK_PARTICLE_SPEED, -BRICK_PARTICLE_SPEED));
    brick_particles.push_back(Object(i_x + 0.5f * CELL_SIZE, i_y + 0.5f * CELL_SIZE, 0.5f * BRICK_PARTICLE_SPEED, -BRICK_PARTICLE_SPEED));
}

void MapManager::add_mail_block_ects(const unsigned short i_x, const unsigned short i_y)
{
    mail_block_ects.push_back(Object(i_x, i_y, 0.0f, ECTS_JUMP_SPEED));
}

void MapManager::draw_map(const bool i_draw_background, const unsigned int i_view_x, sf::RenderWindow& i_window)
{
    unsigned short map_start = std::floor(i_view_x / static_cast<float>(CELL_SIZE));
    unsigned short map_end = std::ceil((SCREEN_WIDTH + i_view_x) / static_cast<float>(CELL_SIZE));
    unsigned short map_height = SCREEN_HEIGHT / CELL_SIZE;

    if (map_end > map.size()) 
    {
        map_end = map.size();
    }

    // Rysowanie wyskakujących punktów ECTS ze skrzynek
    if (!i_draw_background)
    {
        for (const Object& ects : mail_block_ects)
        {
            cell_sprite.setPosition({ects.x - i_view_x, ects.y});
            cell_sprite.setTexture(textures_ects[animation_frame_ects], true);
            // Skalowanie do rozmiaru komórki w razie potrzeby
            sf::Vector2u texture_size = textures_ects[animation_frame_ects].getSize();
            cell_sprite.setScale({static_cast<float>(CELL_SIZE) / texture_size.x, static_cast<float>(CELL_SIZE) / texture_size.y});
            i_window.draw(cell_sprite);
        }
    }

    // Przywrócenie domyślnego skalowania dla rysowania mapy
    cell_sprite.setScale({1.0f, 1.0f});

    for (unsigned short a = map_start; a < map_end; a++)
    {
        for (unsigned short b = 0; b < map_height; b++)
        {
            if (Cell::Empty == map[a][b]) continue;

            cell_sprite.setPosition({static_cast<float>(CELL_SIZE * a - i_view_x), static_cast<float>(CELL_SIZE * b)});
            sf::Vector2u t_size;

            switch (map[a][b])
            {
                case Cell::Floor1:
                    cell_sprite.setTexture(texture_floor1, true);
                    break;
                case Cell::Brick:
                    cell_sprite.setTexture(texture_brick, true);
                    break;
                case Cell::Platform:
                    cell_sprite.setTexture(texture_platform, true);
                    break;
                case Cell::MailBlock:
                    cell_sprite.setTexture(textures_mail_block[animation_frame_mail], true);
                    break;
                case Cell::ActivatedMailBlock:
                    cell_sprite.setTexture(texture_activated_mail, true);
                    break;
                case Cell::Ects:
                    cell_sprite.setTexture(textures_ects[animation_frame_ects], true);
                    break;
                default:
                    continue;
            }

            // Automatyczne dopasowanie skali tekstury do rozmiaru komórki CELL_SIZE (80x80)
            t_size = cell_sprite.getTexture()->getSize();
            cell_sprite.setScale({static_cast<float>(CELL_SIZE) / t_size.x, static_cast<float>(CELL_SIZE) / t_size.y});
            
            i_window.draw(cell_sprite);
        }
    }

    // Rysowanie cząstek rozbitych bloków
    if (!i_draw_background)
    {
        cell_sprite.setTexture(texture_brick, true);
        sf::Vector2u b_size = texture_brick.getSize();
        
        for (const Object& brick_particle : brick_particles)
        {
            cell_sprite.setPosition({brick_particle.x - i_view_x, brick_particle.y});
            // Cząstki rysowane są jako pomniejszone wersje tekstury bazowej
            cell_sprite.setScale({(static_cast<float>(CELL_SIZE) / b_size.x) * 0.5f, (static_cast<float>(CELL_SIZE) / b_size.y) * 0.5f});
            i_window.draw(cell_sprite);
        }
    }
    
    cell_sprite.setScale({1.0f, 1.0f});
}

void MapManager::set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell)
{
    if (i_x < map.size() && i_y < map[0].size())
    {
        map[i_x][i_y] = i_cell;
    }
}

void MapManager::set_map_size(const unsigned short i_new_size)
{
    map.clear();
    map.resize(i_new_size);
    for (auto& column : map)
    {
        column.fill(Cell::Empty);
    }
}

void MapManager::update()
{
    // Aktualizacja klatek animacji na podstawie zegara
    if (animation_clock.getElapsedTime().asMilliseconds() > 150)
    {
        animation_frame_mail = (animation_frame_mail + 1) % 4;
        animation_frame_ects = (animation_frame_ects + 1) % 7;
        animation_clock.restart();
    }

    for (Object& ects : mail_block_ects)
    {
        ects.vertical_speed += GRAVITY;
        ects.y += ects.vertical_speed;
    }

    for (Object& brick_particle : brick_particles)
    {
        brick_particle.vertical_speed += GRAVITY;
        brick_particle.x += brick_particle.horizontal_speed;
        brick_particle.y += brick_particle.vertical_speed;
    }

    brick_particles.erase(std::remove_if(brick_particles.begin(), brick_particles.end(), [](const Object& i_brick_particle)
    {
        return SCREEN_HEIGHT <= i_brick_particle.y;
    }), brick_particles.end());

    mail_block_ects.erase(std::remove_if(mail_block_ects.begin(), mail_block_ects.end(), [](const Object& i_ects)
    {
        return 0.0f <= i_ects.vertical_speed;
    }), mail_block_ects.end());
}

std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox) const
{
    std::vector<unsigned char> output;
    const short top_tile = std::floor(i_hitbox.top / CELL_SIZE);
    const short bottom_tile = std::floor((std::ceil(i_hitbox.height + i_hitbox.top) - 1) / CELL_SIZE);
    const short left_tile = std::floor(i_hitbox.left / CELL_SIZE);
    const short right_tile = std::floor((std::ceil(i_hitbox.left + i_hitbox.width) - 1) / CELL_SIZE);

    for (short a = top_tile; a <= bottom_tile; a++)
    {
        output.push_back(0);
        for (short b = left_tile; b <= right_tile; b++)
        {
            if (0 <= b && b < static_cast<short>(map.size()))
            {
                if (0 <= a && a < static_cast<short>(map[0].size()))
                {
                    if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), map[b][a]))
                    {
                        output[a - top_tile] += std::pow(2, right_tile - b);
                    }
                }
            }
            else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
            {
                output[a - top_tile] += std::pow(2, right_tile - b);
            }
        }
    }
    return output;
}

std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, std::vector<sf::Vector2i>& i_collision_cells, const sf::FloatRect& i_hitbox) const
{
    std::vector<unsigned char> output;
    i_collision_cells.clear();

    const short top_tile = std::floor(i_hitbox.top / CELL_SIZE);
    const short bottom_tile = std::floor((std::ceil(i_hitbox.height + i_hitbox.top) - 1) / CELL_SIZE);
    const short left_tile = std::floor(i_hitbox.left / CELL_SIZE);
    const short right_tile = std::floor((std::ceil(i_hitbox.left + i_hitbox.width) - 1) / CELL_SIZE);

    for (short a = top_tile; a <= bottom_tile; a++)
    {
        output.push_back(0);
        for (short b = left_tile; b <= right_tile; b++)
        {
            if (0 <= b && b < static_cast<short>(map.size()))
            {
                if (0 <= a && a < static_cast<short>(map[0].size()))
                {
                    if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), map[b][a]))
                    {
                        i_collision_cells.push_back(sf::Vector2i(b, a));
                        output[a - top_tile] += std::pow(2, right_tile - b);
                    }
                }
            }
            else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
            {
                output[a - top_tile] += std::pow(2, right_tile - b);
            }
        }
    }
    return output;
}

sf::Color MapManager::get_map_sketch_pixel(const unsigned short i_x, const unsigned short i_y) const
{
    return map_sketch.getPixel({i_x, i_y});
}

Cell MapManager::get_cell_at(unsigned int i_x, unsigned int i_y) const
{
    if (i_x < map.size() && i_y < (SCREEN_HEIGHT / CELL_SIZE))
    {
        return map[i_x][i_y];
    }
    return Cell::Empty;
}