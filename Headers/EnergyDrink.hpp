#pragma once
#include <SFML/Graphics.hpp>
#include "Global.hpp"

class EnergyDrink
{
public:
    EnergyDrink() : x(0), y(0), dead(false) {}
    void update() {}
    void draw(sf::RenderWindow& /*window*/) {}

    sf::FloatRect get_hit_box() const { return sf::FloatRect(sf::Vector2f{x, y}, sf::Vector2f{CELL_SIZE, CELL_SIZE}); }
    void set_dead(bool v) { dead = v; }
    bool get_dead() const { return dead; }

    float x;
    float y;
private:
    bool dead;
};