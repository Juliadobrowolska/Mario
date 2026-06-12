#ifndef MAP_MANAGER_HPP
#define MAP_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <string>
#include "Global.hpp"

enum class Cell : unsigned char
{
    Empty,
    Floor1,
    Floor2,
    Brick,
    MailBlock,
    ActivatedMailBlock,
    Platform
};

class MapManager
{
public:
    MapManager();

    bool initialize();
    bool load_textures();
    bool load_map_from_image(const std::string& i_path);
    void update(unsigned int i_camera_x);
    void draw_map(bool i_background, bool i_foreground, unsigned int i_camera_x, sf::RenderWindow& i_window);

    unsigned int get_map_width() const;

private:
    // Używamy std::vector zawierającego std::array o stałym rozmiarze pionowym 16 klocków
    std::vector<std::array<Cell, 16>> current_map;

    unsigned int animation_frame_counter;
    sf::Clock animation_clock;

    sf::Texture tex_floor1;
    sf::Texture tex_floor2;
    sf::Texture tex_brick;
    sf::Texture tex_platform;
    sf::Texture tex_mail_block;
    sf::Texture tex_activated_mail;
    sf::Texture tex_ects;
};

#endif