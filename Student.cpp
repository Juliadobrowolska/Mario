#include "Headers/Student.hpp"
#include "Headers/Global.hpp"

Student::Student() : 
    sprite(tex_small), 
    x(0.0f), y(0.0f), vx(0.0f), vy(0.0f), dead(false), 
    sugar_overdose(false), is_big(false), energy_drinks_collected(0), 
    jump_timer(0), animation_timer(0.0f), current_frame(0) 
{
    if(!tex_small.loadFromFile("Resources/juststudent.png")) {}
    if(!tex_big.loadFromFile("Resources/happybigstudent.png")) {}
    if(!tex_gameover.loadFromFile("Resources/gameover.png")) {}
    
    if(!tex_run[0].loadFromFile("Resources/run1.png")) {}
    if(!tex_run[1].loadFromFile("Resources/run2.png")) {}
    if(!tex_run[2].loadFromFile("Resources/run3.png")) {}
    
    if(!tex_jump[0].loadFromFile("Resources/jump1.png")) {}
    if(!tex_jump[1].loadFromFile("Resources/jump2.png")) {}
    if(!tex_jump[2].loadFromFile("Resources/jump3.png")) {}
    
    if(!tex_left[0].loadFromFile("Resources/left1.png")) {}
    if(!tex_left[1].loadFromFile("Resources/left2.png")) {}

    sprite.setTexture(tex_small);
}

void Student::reset_stats()
{
    x = CELL_SIZE * 2.0f;
    y = CELL_SIZE * 5.0f;
    vx = 0.0f; vy = 0.0f;
    dead = false;
    is_big = false;
    energy_drinks_collected = 0;
    sprite.setTexture(tex_small);
    sprite.setPosition(sf::Vector2f(x, y));
}

void Student::update(unsigned short map_width, MapManager& map_manager, std::vector<EnergyDrink>& energy_drinks, const std::vector<std::shared_ptr<Enemy>>& enemies)
{
    if (dead) return;

    animation_timer += 1.0f / 60.0f;
    if (animation_timer >= 0.15f) {
        current_frame = (current_frame + 1) % 3;
        animation_timer = 0.0f;
    }

    if (vy != 0.0f) {
        sprite.setTexture(tex_jump[current_frame % 3]);
    }
    else if (vx > 0.0f) { 
        sprite.setTexture(tex_run[current_frame % 3]); 
        sprite.setScale(sf::Vector2f(1.0f, 1.0f)); 
    }
    else if (vx < 0.0f) { 
        sprite.setTexture(tex_left[current_frame % 2]); 
        sprite.setScale(sf::Vector2f(1.0f, 1.0f)); 
    }
    else {
        sprite.setTexture(is_big ? tex_big : tex_small);
    }

    sprite.setPosition(sf::Vector2f(x, y));
}

void Student::die(bool time_out) {
    dead = true;
    sprite.setTexture(tex_gameover);
}

sf::FloatRect Student::get_hitbox() const {
    return sf::FloatRect(sf::Vector2f{x + 2.0f, y + 2.0f}, sf::Vector2f{static_cast<float>(CELL_SIZE) - 4.0f, static_cast<float>(CELL_SIZE) - 4.0f});
}

void Student::draw(sf::RenderWindow& window, unsigned int view_x) {
    sprite.setPosition(sf::Vector2f(x - static_cast<float>(view_x), y));
    window.draw(sprite);
}

void Student::collect_energy_drink() {
    energy_drinks_collected++;
    is_big = true;
}

float Student::get_x() const { return x; }
float Student::get_y() const { return y; }
bool Student::get_dead() const { return dead; }
bool Student::is_sugar_overdose() const { return sugar_overdose; }
unsigned int Student::get_energy_drinks() const { return energy_drinks_collected; }

bool Student::has_queued_enemy() { return !enemy_queue.empty(); }
std::pair<unsigned char, sf::Vector2f> Student::pop_queued_enemy() {
    auto front = enemy_queue.front();
    enemy_queue.pop();
    return front;
}