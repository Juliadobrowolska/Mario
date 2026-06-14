#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <queue>

#include "Global.hpp"
#include "MapManager.hpp"

class EnergyDrink;
class Enemy;

class Student
{
public:
    Student();
    void reset_stats();
    void update(unsigned short map_width, MapManager& map_manager, std::vector<EnergyDrink>& energy_drinks, const std::vector<std::shared_ptr<Enemy>>& enemies);
    void draw(sf::RenderWindow& window, unsigned int view_x);
    void die(bool time_out);
    void collect_energy_drink();
    
    float get_x() const;
    float get_y() const;
    bool get_dead() const;
    bool is_sugar_overdose() const;
    unsigned int get_energy_drinks() const;
    sf::FloatRect get_hitbox() const;

    bool has_queued_enemy();
    std::pair<unsigned char, sf::Vector2f> pop_queued_enemy();

private:
    float x, y, vx, vy;
    bool dead, sugar_overdose, is_big;
    unsigned int energy_drinks_collected;
    int jump_timer;
    float animation_timer;
    int current_frame;

    sf::Texture texture_small, texture_big;
    sf::Sprite sprite;
    
    std::queue<std::pair<unsigned char, sf::Vector2f>> enemy_queue;
};