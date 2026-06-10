#ifndef MAPMANAGER_HPP
#define MAPMANAGER_HPP

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Global.hpp"

class MapManager
{
private:
    Map current_map;

    sf::Texture tex_floor1;
    sf::Texture tex_floor2;
    sf::Texture tex_brick;
    sf::Texture tex_platform;
    sf::Texture tex_mail_block;
    sf::Texture tex_activated_mail;

    sf::Clock animation_clock;
    unsigned int animation_frame;

public:
    MapManager();

    bool initialize();
    bool load_textures();
    bool load_map_from_image(const std::string& i_path);
    void load_level(const std::string& i_level_path);
    void update(const unsigned int i_view_x);
    void draw_map(bool draw_background, bool draw_foreground, const unsigned int i_view_x, sf::RenderWindow& i_window);

    unsigned int get_map_width() const;
    Cell get_cell_at(unsigned int i_x, unsigned int i_y) const;
};

#endif