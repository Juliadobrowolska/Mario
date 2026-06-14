#include <array>
#include <cmath>
#include <algorithm>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/EnergyDrink.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/Student.hpp"

Student::Student() :
    x(0.0f),
    y(0.0f),
    vx(0.0f),
    vy(0.0f),
    dead(false),
    sugar_overdose(false),
    is_big(false),
    energy_drinks_collected(0),
    jump_timer(0),
    animation_timer(0.0f),
    current_frame(0),
    sprite(texture_small)
{
    texture_small.loadFromFile("Resources/juststudent.png");
    texture_big.loadFromFile("Resources/happybigstudent.png");
    sprite.setTexture(texture_small);
}

void Student::reset_stats()
{
    x = CELL_SIZE * 2.0f;
    y = CELL_SIZE * 10.0f;
    vx = 0.0f;
    vy = 0.0f;
    dead = false;
    sugar_overdose = false;
    is_big = false;
    energy_drinks_collected = 0;
    jump_timer = 0;
    
    sprite.setTexture(texture_small);
    sprite.setPosition(sf::Vector2f{x, y});
}

void Student::update(unsigned short map_width, MapManager& map_manager, std::vector<EnergyDrink>& energy_drinks, std::vector<std::shared_ptr<Enemy>>& enemies)
{
    if (dead) return;

    bool move_left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    bool move_right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
    bool jump_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);

    if (move_left)
    {
        vx = -STUDENT_WALK_SPEED;
    }
    else if (move_right)
    {
        vx = STUDENT_WALK_SPEED;
    }
    else
    {
        vx = 0.0f;
    }

    vy += GRAVITY;
    if (vy > MAX_VERTICAL_SPEED) vy = MAX_VERTICAL_SPEED;

    sf::FloatRect ground_check_hitbox = get_hitbox();
    ground_check_hitbox.position.y += 1.0f;
    auto ground_collisions = map_manager.map_collision({ Cell::Wall, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::ActivatedMailBlock }, ground_check_hitbox);
    bool on_ground = !ground_collisions.empty() && ground_collisions[0] == 1;

    if (on_ground)
    {
        vy = 0.0f;
        if (jump_pressed)
        {
            vy = -STUDENT_JUMP_SPEED;
            jump_timer = 10;
        }
    }
    else if (jump_pressed && jump_timer > 0)
    {
        vy = -STUDENT_JUMP_SPEED;
        jump_timer--;
    }
    else
    {
        jump_timer = 0;
    }

    x += vx;
    if (x < 0) x = 0;
    if (x > (map_width * CELL_SIZE) - CELL_SIZE) x = (map_width * CELL_SIZE) - CELL_SIZE;
    
    sf::FloatRect hitbox = get_hitbox();
    auto x_collisions = map_manager.map_collision({ Cell::Wall, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::ActivatedMailBlock }, hitbox);
    if (!x_collisions.empty() && x_collisions[0] == 1)
    {
        x -= vx;
        vx = 0.0f;
    }

    y += vy;
    hitbox = get_hitbox();
    auto y_collisions = map_manager.map_collision({ Cell::Wall, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::ActivatedMailBlock }, hitbox);
    if (!y_collisions.empty() && y_collisions[0] == 1)
    {
        if (vy > 0.0f)
        {
            y -= vy;
            vy = 0.0f;
        }
        else if (vy < 0.0f)
        {
            y -= vy;
            vy = 0.0f;

            unsigned short tile_x = static_cast<unsigned short>((hitbox.position.x + hitbox.size.x / 2.0f) / CELL_SIZE);
            unsigned short tile_y = static_cast<unsigned short>(hitbox.position.y / CELL_SIZE);
            
            if (map_manager.get_cell(tile_x, tile_y) == Cell::MailBlock)
            {
                map_manager.set_cell(tile_x, tile_y, Cell::ActivatedMailBlock);
                energy_drinks.push_back(EnergyDrink(tile_x * CELL_SIZE, (tile_y - 1) * CELL_SIZE));
            }
        }
    }

    if (y > SCREEN_HEIGHT)
    {
        die(false);
    }

    // --- SYSTEM ANIMACJI Z UWZGLĘDNIENIEM STRON LEWO/PRAWO ---
    animation_timer += 1.0f / 60.0f;
    if (animation_timer >= 0.15f)
    {
        current_frame = (current_frame + 1) % 3;
        animation_timer = 0.0f;
    }

    if (!on_ground) // Skok
    {
        if (is_big)
        {
            texture_big.loadFromFile("Resources/bigstudentjump" + std::to_string((current_frame % 3) + 1) + ".png");
            sprite.setTexture(texture_big);
        }
        else
        {
            texture_small.loadFromFile("Resources/jump" + std::to_string((current_frame % 3) + 1) + ".png");
            sprite.setTexture(texture_small);
        }
    }
    else if (vx > 0.0f) // Bieg w prawo (run1, run2, run3)
    {
        if (is_big)
        {
            texture_big.loadFromFile("Resources/bigrun" + std::to_string((current_frame % 2) + 1) + ".png");
            sprite.setTexture(texture_big);
        }
        else
        {
            texture_small.loadFromFile("Resources/run" + std::to_string(current_frame + 1) + ".png");
            sprite.setTexture(texture_small);
        }
        sprite.setScale(sf::Vector2f{1.0f, 1.0f});
        sprite.setOrigin(sf::Vector2f{0.0f, 0.0f});
    }
    else if (vx < 0.0f) // Bieg w lewo (left1, left2)
    {
        if (is_big)
        {
            // Duży student korzysta z odbicia lustrzanego bigrun, bo nie ma osobnych plików left dla dużego
            texture_big.loadFromFile("Resources/bigrun" + std::to_string((current_frame % 2) + 1) + ".png");
            sprite.setTexture(texture_big);
            sprite.setScale(sf::Vector2f{-1.0f, 1.0f});
            sprite.setOrigin(sf::Vector2f{sprite.getLocalBounds().size.x, 0.0f});
        }
        else
        {
            // Mały Student ładuje dedykowane pliki left1 i left2!
            texture_small.loadFromFile("Resources/left" + std::to_string((current_frame % 2) + 1) + ".png");
            sprite.setTexture(texture_small);
            sprite.setScale(sf::Vector2f{1.0f, 1.0f});
            sprite.setOrigin(sf::Vector2f{0.0f, 0.0f});
        }
    }
    else // Stanie w miejscu
    {
        if (is_big)
        {
            texture_big.loadFromFile("Resources/happybigstudent.png");
            sprite.setTexture(texture_big);
        }
        else
        {
            texture_small.loadFromFile("Resources/juststudent.png");
            sprite.setTexture(texture_small);
        }
        sprite.setScale(sf::Vector2f{1.0f, 1.0f});
        sprite.setOrigin(sf::Vector2f{0.0f, 0.0f});
    }

    sprite.setPosition(sf::Vector2f{x, y});
}

void Student::collect_energy_drink()
{
    energy_drinks_collected++;
    is_big = true;

    if (energy_drinks_collected >= 3)
    {
        sugar_overdose = true;
        die(false);
    }
}

void Student::die(bool time_out)
{
    dead = true;
    if (sugar_overdose)
    {
        // Obsługiwane przez tło w main.cpp
    }
    else
    {
        // Zarówno dla potrącenia przez wroga jak i końca czasu ładujemy gameover.png (smutny student pod ścianą)
        texture_small.loadFromFile("Resources/gameover.png");
        sprite.setTexture(texture_small);
    }
}

sf::FloatRect Student::get_hitbox() const
{
    float width = CELL_SIZE - 4.0f;
    float height = is_big ? (CELL_SIZE * 2.0f) - 4.0f : CELL_SIZE - 2.0f;
    return sf::FloatRect(sf::Vector2f{x + 2.0f, is_big ? y - CELL_SIZE + 4.0f : y + 2.0f}, sf::Vector2f{width, height});
}

float Student::get_x() const { return x; }
float Student::get_y() const { return y; }
bool Student::get_dead() const { return dead; }
bool Student::is_sugar_overdose() const { return sugar_overdose; }
unsigned int Student::get_energy_drinks() const { return energy_drinks_collected; }

bool Student::has_queued_enemy() { return !enemy_queue.empty(); }
std::pair<unsigned char, sf::Vector2f> Student::pop_queued_enemy()
{
    auto front = enemy_queue.front();
    enemy_queue.pop();
    return front;
}

void Student::draw(sf::RenderWindow& window, unsigned int view_x)
{
    sf::Vector2f original_pos = sprite.getPosition();
    sprite.setPosition(sf::Vector2f{original_pos.x - static_cast<float>(view_x), original_pos.y});
    window.draw(sprite);
    sprite.setPosition(original_pos);
}