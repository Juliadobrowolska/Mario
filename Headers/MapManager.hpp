#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <random>
#include "Global.hpp"

class MapManager
{
private:
    std::vector<Object> brick_particles;
    std::vector<Object> mail_block_ects;

    sf::Image map_sketch;
    sf::Sprite cell_sprite;

    // Tekstury ładowane z pojedynczych plików użytkownika
    sf::Texture texture_floor1;
    sf::Texture texture_brick;
    sf::Texture texture_platform;
    sf::Texture texture_activated_mail;
    
    // Tablice tekstur dla elementów animowanych
    std::array<sf::Texture, 5> textures_mail_block;
    std::array<sf::Texture, 7> textures_ects;

    unsigned int animation_frame_mail;
    unsigned int animation_frame_ects;
    sf::Clock animation_clock;

    Map map;

    // Zarządzanie poziomami i losowością
    unsigned char current_level;
    std::vector<unsigned char> available_levels;
    std::mt19937 random_engine;

public:
    MapManager();

    unsigned short get_map_sketch_height() const;
    unsigned short get_map_sketch_width() const;
    unsigned short get_map_width() const;

    void init_level_system();
    void load_next_random_level();
    void load_current_level_sketch();

    void add_brick_particles(const unsigned short i_x, const unsigned short i_y);
    void add_mail_block_ects(const unsigned short i_x, const unsigned short i_y);
    void draw_map(const bool i_draw_background, const unsigned int i_view_x, sf::RenderWindow& i_window);
    void set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell);
    void set_map_size(const unsigned short i_new_size);
    void update();

    std::vector<unsigned char> map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox) const;
    std::vector<unsigned char> map_collision(const std::vector<Cell>& i_check_cells, std::vector<sf::Vector2i>& i_collision_cells, const sf::FloatRect& i_hitbox) const;

    sf::Color get_map_sketch_pixel(const unsigned short i_x, const unsigned short i_y) const;
    Cell get_cell_at(unsigned int i_x, unsigned int i_y) const;
};