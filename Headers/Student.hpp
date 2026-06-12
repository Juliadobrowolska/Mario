#ifndef STUDENT_HPP
#define STUDENT_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "MapManager.hpp"

class Student
{
public:
    Student();

    bool load_textures();
    void update(const unsigned i_view_x, MapManager& i_map_manager);
    void draw(sf::RenderWindow& i_window, unsigned int i_camera_x);

    void set_position(const float i_x, const float i_y);
    void set_vertical_speed(const float i_value);
    void die(const bool i_instant_death);
    void reset();
    void draw_energy_drinks(const unsigned i_view_x, sf::RenderWindow& i_window);

    float get_x() const;
    float get_vertical_speed() const;
    sf::FloatRect get_hit_box() const;

private:
    bool is_dead;
    bool crouching;
    bool flipped;
    bool on_ground;
    bool can_jump;

    float enemy_bounce_speed;
    float horizontal_speed;
    float vertical_speed;
    float x;
    float y;

    unsigned char jump_timer;
    unsigned char powerup_state;

    unsigned short death_timer;
    unsigned short turbo_student_timer;
    unsigned short immunity_timer;

    unsigned int walk_frame_counter;

    sf::Clock animation_clock;
    sf::Font game_over_font;
    sf::Text game_over_text;

    sf::Texture tex_idle;
    sf::Texture tex_jump_up;
    sf::Texture tex_jump_down;
    sf::Texture tex_death;

    std::vector<sf::Texture> tex_normal_walk;
    std::vector<sf::Texture> tex_big_walk;

    sf::Sprite sprite;
};

#endif