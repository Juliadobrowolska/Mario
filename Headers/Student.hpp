#pragma once

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include "Animation.hpp"
#include "Global.hpp"
#include "EnergyDrink.hpp"
#include "MapManager.hpp"

class Student
{
private:
    bool is_dead = false;
    bool crouching = false;
    bool flipped = false;
    bool on_ground = false;

    bool can_jump = true;

    float enemy_bounce_speed = 0.0f;
    float horizontal_speed = 0.0f;
    float vertical_speed = 0.0f;
    float x = 0.0f;
    float y = 0.0f;

    unsigned char jump_timer = 0;
    unsigned char powerup_state = 0;

    unsigned short death_timer = 0;
    unsigned short turbo_student_timer = 0;
    unsigned short immunity_timer = 0;

    std::vector<EnergyDrink> energy_drinks;
    std::array<Animation, 2> walk_animations;

    std::vector<sf::Texture> tex_normal_walk;
    std::vector<sf::Texture> tex_big_walk;
    sf::Texture tex_jump_up;
    sf::Texture tex_jump_down;
    sf::Texture tex_death;
    sf::Texture tex_idle;

    sf::Sprite sprite;
    sf::Font game_over_font;
    sf::Text game_over_text;
    sf::Clock animation_clock;
    unsigned int walk_frame_counter = 0;

public:
    Student();

    float get_vertical_speed() const;
    float get_x() const;
    sf::FloatRect get_hit_box() const;

    bool load_textures();
    void die(const bool i_instant_death);

    void draw(sf::RenderWindow& i_window);
    void draw_energy_drinks(const unsigned i_view_x, sf::RenderWindow& i_window);
    void reset();
    void set_position(const float i_x, const float i_y);
    void set_vertical_speed(const float i_value);

    void update(const unsigned i_view_x, MapManager& i_map_manager);
};
